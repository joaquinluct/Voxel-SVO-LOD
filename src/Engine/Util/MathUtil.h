#pragma once

inline int GetNext16Multiple(int value) {
    // Calculamos el resto de la división por 16.
    int resto = value % 16;

    // Si el resto es 0, el número ya es un múltiplo de 16.
    if (resto == 0) {
        return value;
    }
    else {
        // Si no, sumamos la diferencia para llegar al siguiente múltiplo.
        // El resultado será el múltiplo de 16 mayor más cercano.
        return value + (16 - resto);
    }
}
