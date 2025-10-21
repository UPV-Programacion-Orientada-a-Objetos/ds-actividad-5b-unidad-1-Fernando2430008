#include <iostream>

using namespace std;

// clase base abstracta
template <typename T>
class MatrizBase {
protected:
    int _filas;
    int _columnas;

public:
    MatrizBase(int f, int c) : _filas(f), _columnas(c) {}
    virtual ~MatrizBase() {}

    // interfaz requerida
    virtual void cargarValores() = 0;
    virtual MatrizBase<T>* sumar(const MatrizBase<T>& otra) const = 0;
    virtual void imprimir() const = 0;

    // utilidades genericas para acceso a datos
    virtual T getValor(int i, int j) const = 0;
    virtual void setValor(int i, int j, const T& v) = 0;

    // operador + natural que delega al metodo virtual sumar
    MatrizBase<T>* operator+(const MatrizBase<T>& otra) const {
        return this->sumar(otra);
    }

    int filas() const { return _filas; }
    int columnas() const { return _columnas; }
};

// matriz dinamica con punteros, regla de los cinco
template <typename T>
class MatrizDinamica : public MatrizBase<T> {
private:
    T **_datos;

    void reservar(int f, int c) {
        this->_filas = f;
        this->_columnas = c;
        _datos = new T*[f];
        for (int i = 0; i < f; ++i) {
            _datos[i] = new T[c];
        }
    }

    void liberar() {
        if (_datos != nullptr) {
            for (int i = 0; i < this->_filas; ++i) {
                delete[] _datos[i];
            }
            delete[] _datos;
            _datos = nullptr;
        }
        this->_filas = 0;
        this->_columnas = 0;
    }

public:
    MatrizDinamica(int f, int c) : MatrizBase<T>(f, c), _datos(nullptr) {
        reservar(f, c);
        // inicializar a cero
        for (int i = 0; i < f; ++i) {
            for (int j = 0; j < c; ++j) {
                _datos[i][j] = T();
            }
        }
    }

    // constructor de copia (copia profunda)
    MatrizDinamica(const MatrizDinamica& otra) : MatrizBase<T>(otra._filas, otra._columnas), _datos(nullptr) {
        reservar(otra._filas, otra._columnas);
        for (int i = 0; i < this->_filas; ++i) {
            for (int j = 0; j < this->_columnas; ++j) {
                _datos[i][j] = otra._datos[i][j];
            }
        }
    }

    // operador asignacion copia
    MatrizDinamica& operator=(const MatrizDinamica& otra) {
        if (this != &otra) {
            liberar();
            reservar(otra._filas, otra._columnas);
            for (int i = 0; i < this->_filas; ++i) {
                for (int j = 0; j < this->_columnas; ++j) {
                    _datos[i][j] = otra._datos[i][j];
                }
            }
        }
        return *this;
    }

    // constructor de movimiento
    MatrizDinamica(MatrizDinamica&& otra) noexcept : MatrizBase<T>(0, 0), _datos(nullptr) {
        this->_filas = otra._filas;
        this->_columnas = otra._columnas;
        _datos = otra._datos;
        otra._filas = 0;
        otra._columnas = 0;
        otra._datos = nullptr;
    }

    // asignacion por movimiento
    MatrizDinamica& operator=(MatrizDinamica&& otra) noexcept {
        if (this != &otra) {
            liberar();
            this->_filas = otra._filas;
            this->_columnas = otra._columnas;
            _datos = otra._datos;
            otra._filas = 0;
            otra._columnas = 0;
            otra._datos = nullptr;
        }
        return *this;
    }

    ~MatrizDinamica() {
        // mensajes para demostrar la liberacion de memoria
        cout << "liberando memoria (matriz dinamica)..." << endl;
        liberar();
    }

    void cargarValores() override {
        // ejemplo: no lee de consola, solo pone ceros para mantener simple
        for (int i = 0; i < this->_filas; ++i) {
            for (int j = 0; j < this->_columnas; ++j) {
                _datos[i][j] = T();
            }
        }
    }

    MatrizBase<T>* sumar(const MatrizBase<T>& otra) const override {
        if (this->_filas != otra.filas() || this->_columnas != otra.columnas()) {
            cout << "error: dimensiones incompatibles para suma" << endl;
            return 0;
        }
        MatrizDinamica<T>* res = new MatrizDinamica<T>(this->_filas, this->_columnas);
        for (int i = 0; i < this->_filas; ++i) {
            for (int j = 0; j < this->_columnas; ++j) {
                T v = this->getValor(i, j) + otra.getValor(i, j);
                res->setValor(i, j, v);
            }
        }
        return res;
    }

    void imprimir() const override {
        for (int i = 0; i < this->_filas; ++i) {
            cout << "| ";
            for (int j = 0; j < this->_columnas; ++j) {
                cout << _datos[i][j];
                if (j < this->_columnas - 1) cout << " | ";
            }
            cout << " |" << endl;
        }
    }

    T getValor(int i, int j) const override {
        return _datos[i][j];
    }

    void setValor(int i, int j, const T& v) override {
        _datos[i][j] = v;
    }
};

// matriz estatica con tamanos fijos en tiempo de compilacion
template <typename T, int M, int N>
class MatrizEstatica : public MatrizBase<T> {
private:
    T _datos[M][N];

public:
    MatrizEstatica() : MatrizBase<T>(M, N) {
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < N; ++j) {
                _datos[i][j] = T();
            }
        }
    }

    ~MatrizEstatica() {
        cout << "destruyendo objeto (matriz estatica)" << endl;
    }

    void cargarValores() override {
        // ejemplo: pone ceros
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < N; ++j) {
                _datos[i][j] = T();
            }
        }
    }

    MatrizBase<T>* sumar(const MatrizBase<T>& otra) const override {
        if (this->_filas != otra.filas() || this->_columnas != otra.columnas()) {
            cout << "error: dimensiones incompatibles para suma" << endl;
            return 0;
        }
        MatrizEstatica<T, M, N>* res = new MatrizEstatica<T, M, N>();
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < N; ++j) {
                res->setValor(i, j, this->getValor(i, j) + otra.getValor(i, j));
            }
        }
        return res;
    }

    void imprimir() const override {
        for (int i = 0; i < M; ++i) {
            cout << "| ";
            for (int j = 0; j < N; ++j) {
                cout << _datos[i][j];
                if (j < N - 1) cout << " | ";
            }
            cout << " |" << endl;
        }
    }

    T getValor(int i, int j) const override {
        return _datos[i][j];
    }

    void setValor(int i, int j, const T& v) override {
        _datos[i][j] = v;
    }
};

int main() {
    // demostracion polimorfica con tipo float
    cout << "--- sistema generico de algebra lineal ---" << endl;
    cout << endl;
    cout << ">> demostracion de genericidad (tipo float) <<" << endl;
    cout << endl;

    cout << "creando matriz dinamica a (3x2)..." << endl;
    MatrizBase<float>* A = new MatrizDinamica<float>(3, 2);
    // asignar valores manualmente
    A->setValor(0, 0, 1.5f); A->setValor(0, 1, 2.0f);
    A->setValor(1, 0, 0.0f); A->setValor(1, 1, 1.0f);
    A->setValor(2, 0, 4.5f); A->setValor(2, 1, 3.0f);
    cout << "a =" << endl;
    A->imprimir();
    cout << endl;

    cout << "creando matriz estatica b (3x2)..." << endl;
    MatrizBase<float>* B = new MatrizEstatica<float, 3, 2>();
    B->setValor(0, 0, 0.5f); B->setValor(0, 1, 1.0f);
    B->setValor(1, 0, 2.0f); B->setValor(1, 1, 3.0f);
    B->setValor(2, 0, 1.0f); B->setValor(2, 1, 1.0f);
    cout << "b =" << endl;
    B->imprimir();
    cout << endl;

    cout << "sumando: matriz c = a + b ..." << endl;
    cout << "(la suma es manejada por el metodo virtual de matriz dinamica si a es dinamica)" << endl;
    MatrizBase<float>* C = (*A) + (*B);
    cout << endl;
    if (C != 0) {
        cout << "matriz resultado c (3x2, tipo float):" << endl;
        C->imprimir();
    }
    cout << endl;

    cout << ">> demostracion de genericidad (tipo int) <<" << endl;
    cout << endl;
    cout << "creando matriz dinamica a2 (2x2)..." << endl;
    MatrizBase<int>* A2 = new MatrizDinamica<int>(2, 2);
    A2->setValor(0, 0, 1); A2->setValor(0, 1, 2);
    A2->setValor(1, 0, 3); A2->setValor(1, 1, 4);
    cout << "a2 =" << endl;
    A2->imprimir();
    cout << endl;

    cout << "creando matriz estatica b2 (2x2)..." << endl;
    MatrizBase<int>* B2 = new MatrizEstatica<int, 2, 2>();
    B2->setValor(0, 0, 5); B2->setValor(0, 1, 6);
    B2->setValor(1, 0, 7); B2->setValor(1, 1, 8);
    cout << "b2 =" << endl;
    B2->imprimir();
    cout << endl;

    cout << "sumando: matriz c2 = a2 + b2 ..." << endl;
    MatrizBase<int>* C2 = (*A2) + (*B2);
    if (C2 != 0) {
        cout << "matriz resultado c2 (2x2, tipo int):" << endl;
        C2->imprimir();
    }
    cout << endl;

    cout << ">> demostracion de limpieza de memoria <<" << endl;
    // limpieza explicita
    cout << "eliminando c..." << endl;
    delete C; C = 0;
    cout << "eliminando a..." << endl;
    delete A; A = 0;
    cout << "eliminando b..." << endl;
    delete B; B = 0;
    cout << "eliminando c2..." << endl;
    delete C2; C2 = 0;
    cout << "eliminando a2..." << endl;
    delete A2; A2 = 0;
    cout << "eliminando b2..." << endl;
    delete B2; B2 = 0;
    cout << "sistema cerrado." << endl;

    return 0;
}
