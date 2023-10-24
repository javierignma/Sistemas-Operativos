#include <iostream>
#include <vector>
#include <experimental/filesystem>
#include <fstream>
#include <cstdlib>
#include <algorithm>

namespace fs = std::experimental::filesystem;

int id_max = 0;

std::vector < std::string > split_path(const std::string & path, std::string delimiter) {
    std::vector < std::string > result;
    size_t pos = 0;
    std::string token;
    std::string remaining_path = path; // Almacenar el path original para no modificarlo directamente
    while ((pos = remaining_path.find(delimiter)) != std::string::npos) {
        token = remaining_path.substr(0, pos);
        result.push_back(token);
        remaining_path = remaining_path.substr(pos + delimiter.length()); // Actualizar remaining_path
    }
    result.push_back(remaining_path);
    return result;
}

std::string go_back(std::string ruta) {
    std::vector < std::string > lista = split_path(ruta, "/");
    std::string retorno = "/";

    for (std::string dir: lista) {
        if (dir == lista.back()) break;
        retorno = fs::path(retorno) / dir;
    }

    return retorno;
}


//ID 1: Creación de la estructura.
class TreeNode {
    public: std::string nombre_carpeta;
    std::string ruta;
    std::vector < TreeNode * > children;
    TreeNode * parent;
    int id;

    TreeNode() {
        nombre_carpeta = "root";
        parent = nullptr;
        id = id_max;
        id_max++;
    }

    int get_level() {
        int level = 0;
        TreeNode * p = parent;
        while (p) {
            level += 1;
            p = p -> parent;
        }
        return level;
    }

    void print_tree() {
        std::string spaces = std::string(get_level() * 3, ' ');
        std::string prefix = parent ? "|__" : "";
        std::cout << prefix << ruta << std::endl;
        for (TreeNode * child: children) {
            child -> print_tree();
        }
    }

    bool add_child(TreeNode * child, std::string actual_path) {
        std::vector < std::string > ruta_split = split_path(child -> ruta, "/");
        std::string new_file = ruta_split.back();

        if (ruta == actual_path) {
            child -> parent = this;
            child -> nombre_carpeta = new_file;
            children.push_back(child);
            std::cout << "Nodo añadido." << std::endl;
            return true;
        } else {
            for (TreeNode * hijo: children) {
                bool valor = hijo -> add_child(child, actual_path);
                if (valor) {
                    return true;
                }
            }
        }
        
        return false;
    }

    bool delete_child(TreeNode * child) {
        // Verificar si el hijo a eliminar tiene la misma ruta
        auto it = std::find_if(children.begin(), children.end(), [child](TreeNode * hijo) {
            return hijo -> ruta == child -> ruta;
        });

        if (it != children.end()) {
            children.erase(it);
            return true; // Elemento eliminado exitosamente
        }

        // Buscar el hijo a eliminar en los hijos de los hijos (recursivamente)
        for (TreeNode * hijo: children) {
            if (hijo -> delete_child(child)) {
                return true; // Elemento eliminado exitosamente
            }
        }

        return false; // El elemento no se encontró en el árbol
    }

    void lsRecursivo(const std::string & ruta_base) {
        if (ruta == ruta_base) {
            std::cout << ruta << ":" << std::endl;
            verHijos();
            if (children.empty()) {
                return;
            } else {
                for (TreeNode * hijo: children) {
                    hijo -> lsRecursivo(hijo -> ruta);
                }
            }
        } else {
            for (TreeNode * hijo: children) {
                if (hijo -> ruta == ruta_base) {
                    std::cout << hijo -> ruta << ":" << std::endl;
                    hijo -> verHijos();
                    if (hijo -> children.empty()) {
                        return;
                    } else {
                        for (TreeNode * hijo2: hijo -> children) {
                            hijo2 -> lsRecursivo(hijo2 -> ruta);
                        }
                    }
                }
            }
        }
    }

    void lsNormal(const std::string & ruta_base) {
        if (ruta == ruta_base) {
            verHijos();
        } else {
            for (TreeNode * hijo: children) {
                if (hijo -> ruta == ruta_base) {
                    hijo -> verHijos();
                    return;
                }
            }
            for (TreeNode * hijo: children) {
                hijo -> lsNormal(ruta_base);
            }
        }
    }

    void ls_inodos(const std::string & ruta_base) {
        if (ruta == ruta_base) {
            verHijos_inodos();
        } else {
            for (TreeNode * hijo: children) {
                if (hijo -> ruta == ruta_base) {
                    hijo -> verHijos_inodos();
                    return;
                }
            }
            for (TreeNode * hijo: children) {
                hijo -> ls_inodos(ruta_base);
            }
        }
    }

    bool existeRuta(TreeNode * child) {
        for (TreeNode * hijo: children) {
            if (hijo -> ruta == child -> ruta) {
                return true;
            } else {
                if (hijo -> existeRuta(child)) {
                    return true;
                }
            }
        }
        return false;
    }

    bool existeHijo(TreeNode * child,
        const std::string & nombre) {
        for (TreeNode * hijo: children) {
            if (hijo -> ruta == child -> ruta) {
                if (hijo -> nombre_carpeta == nombre) {
                    return true;
                }
            } else {
                if (hijo -> existeHijo(child, nombre)) {
                    return true;
                }
            }
        }
        return false;
    }

    void verHijos() {
        if (children.empty()) {
            return;
        } else {
            for (TreeNode * hijo: children) {
                std::cout << "-" << hijo -> nombre_carpeta << std::endl;
            }
        }
    }

    void verHijos_inodos() {
        if (children.empty()) {
            return;
        } else {
            for (TreeNode * hijo: children) {
                std::cout << "- " << hijo -> id << " " << hijo -> nombre_carpeta << std::endl;
            }
        }
    }

    void mostrarDatos() {
        std::cout << "ruta: " << ruta << std::endl;
        std::cout << "nombre: " << nombre_carpeta << std::endl;
    }

};

int main() {
    std::string ruta_base = "/root";
    std::string absolute_path = fs::current_path().string();
    TreeNode * root = new TreeNode();
    root -> ruta = ruta_base;
    bool flag = true;

    while (flag) {
        std::string opcion_original;
        std::cout << ruta_base << "$ ";
        std::getline(std::cin, opcion_original);
        std::vector < std::string > opcion = split_path(opcion_original, " ");
        int parametros = opcion.size();

        if (parametros == 1) {
            if (opcion[0] == "ls") {//ID 5: ls
                root -> lsNormal(ruta_base);
            } else if (opcion[0] == "exit") {
                flag = false;
            } else if (opcion[0] == "ver") { //ID 10: Seguir directorio de trabajo.
                root -> print_tree();
            } else if (opcion[0] == "clear") {
                system("clear");
            }
        } else {
            if (opcion[0] == "ls" && opcion[1] == "-r" || opcion[1] == "-R") {//ID 6: ls -r
                root -> lsRecursivo(ruta_base);
            } else if (opcion[0] == "mkdir") { //ID 3: mkdir
                std::string path_aux = fs::path(ruta_base) / opcion[1];
                std::string path_aux2 = fs::path(absolute_path) / opcion[1];
                TreeNode * nodo = new TreeNode();
                nodo -> ruta = path_aux;
                nodo -> nombre_carpeta = opcion[1];
                bool valor = root -> add_child(nodo, ruta_base);
                if (valor) {
                    fs::create_directory(path_aux2);
                    std::cout << "Carpeta " << opcion[1] << " fue creada con éxito..." << std::endl;
                }
            } else if (opcion[0] == "cd" && opcion[1] != "..") {//ID 7: cd
                std::string path_aux = fs::path(ruta_base) / opcion[1];
                std::string path_aux2 = fs::path(absolute_path) / opcion[1];
                TreeNode * nodo_prueba = new TreeNode();
                nodo_prueba -> ruta = path_aux;
                bool valor = root -> existeRuta(nodo_prueba);
                if (valor) {
                    ruta_base = path_aux;
                    absolute_path = path_aux2;
                } else {
                    std::cout << "bash: cd: " << opcion[1] << ": No existe el archivo o directorio" << std::endl;
                }
            } else if (opcion[0] == "cd" && opcion[1] == ".." && ruta_base != "root" && ruta_base != "/root") {
                absolute_path = go_back(absolute_path);//ID 7: cd ..
                ruta_base = go_back(ruta_base);
            } else if (opcion[0] == "rm" && opcion[1] != "-r" && opcion[1] != "-R") { //ID 8: rm
                std::vector < std::string > aux = split_path(opcion[1], ".");
                if (aux.size() != 2) {
                    std::cout << "rm: no se puede borrar '" << opcion[1] << "': Es un directorio" << std::endl;
                } else {
                    std::string path_aux = fs::path(ruta_base) / opcion[1];
                    std::string path_aux2 = fs::path(absolute_path) / opcion[1];
                    TreeNode * nodo_prueba = new TreeNode();
                    nodo_prueba -> ruta = path_aux;
                    bool valor = root -> delete_child(nodo_prueba);
                    if (valor) {
                        fs::remove(path_aux2);
                    } else {
                        std::cout << "rm: fallo al borrar '" << opcion[1] << "': No existe el archivo o directorio" << std::endl;
                    }
                }
            } else if (opcion[0] == "rm" && opcion[1] == "-r" || opcion[1] == "-R") {//ID 8: rm -r
                std::string path_aux = fs::path(ruta_base) / opcion[2];
                std::string path_aux2 = fs::path(absolute_path) / opcion[2];
                TreeNode * nodo_prueba = new TreeNode();
                nodo_prueba -> ruta = path_aux;
                bool valor = root -> delete_child(nodo_prueba);
                if (valor) {
                    fs::remove(path_aux2);
                    std::cout << "Eliminado con éxito..." << std::endl;
                } else {
                    std::cout << "rm: fallo al borrar '" << opcion[1] << "': No existe el archivo o directorio" << std::endl;
                }
            } else if (opcion[0] == "ls" && opcion[1] == "-i") {//ID 5: ls -i
                root -> ls_inodos(ruta_base);
            } else if (opcion[0] == "touch") { //ID 2: Touch
                std::string path_aux = fs::path(ruta_base) / opcion[1];
                std::string path_aux2 = fs::path(absolute_path) / opcion[1];
                TreeNode * nodo = new TreeNode();
                nodo -> nombre_carpeta = opcion[1];
                nodo -> ruta = path_aux;
                bool valor = root -> add_child(nodo, ruta_base);
                if (valor) {
                    try {
                        fs::path ruta_archivo(path_aux2);
                        if (fs::exists(ruta_archivo)) {
                            std::cout << "El archivo ya existe.\n";
                        } else {
                            std::ofstream archivo(ruta_archivo);

                            if (archivo.is_open()) {
                                archivo.close();
                            }
                            std::cout << "Archivo " << opcion[1] << " fue creado con éxito..." << std::endl;
                        }
                    } catch (const fs::filesystem_error & ex) {
                        std::cout << "Error de filesystem: " << ex.what() << '\n';
                    }
                }
            }
        }
    }

    return 0;
}