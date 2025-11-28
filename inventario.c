#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PRODUCTOS 100
#define NOMBRE_LEN 50
#define INVENTARIO_FILE "inventario.txt"
#define VENTAS_FILE "ventas.txt"

typedef struct {
    char nombre[NOMBRE_LEN];
    int cantidad;
    float precio;
} Producto;

typedef struct {
    char nombre[NOMBRE_LEN];
    int cantidad;
    float total;
} Venta;

Producto inventario[MAX_PRODUCTOS];
int total_productos = 0;

void minusculas(const char *src, char *dst) {
    for (int i = 0; src[i]; i++) dst[i] = tolower((unsigned char)src[i]);
    dst[strlen(src)] = '\0';
}

int comparar_insensible(const char *a, const char *b) {
    char sa[NOMBRE_LEN], sb[NOMBRE_LEN];
    minusculas(a, sa);
    minusculas(b, sb);
    return strcmp(sa, sb);
}

void cargar_inventario() {
    FILE *f = fopen(INVENTARIO_FILE, "r");
    total_productos = 0;
    if (!f) return;
    while (!feof(f) && total_productos < MAX_PRODUCTOS) {
        Producto p;
        if (fscanf(f, "%49[^;];%d;%f\n", p.nombre, &p.cantidad, &p.precio) == 3) {
            inventario[total_productos++] = p;
        } else break;
    }
    fclose(f);
}

void guardar_inventario() {
    FILE *f = fopen(INVENTARIO_FILE, "w");
    if (!f) {
        printf("Error al guardar inventario.\n");
        return;
    }
    for (int i = 0; i < total_productos; i++) {
        fprintf(f, "%s;%d;%.2f\n", inventario[i].nombre, inventario[i].cantidad, inventario[i].precio);
    }
    fclose(f);
}

void listar_productos() {
    printf("\n--- INVENTARIO OJITOS BONITOS ---\n");
    if (total_productos == 0) {
        printf("No hay productos registrados.\n");
        return;
    }
    for (int i = 0; i < total_productos; i++) {
        printf("%d) %s | Cantidad: %d | Precio: %.2f\n", i+1, inventario[i].nombre, inventario[i].cantidad, inventario[i].precio);
    }
}

int buscar_indice_por_nombre(const char *nombre) {
    for (int i = 0; i < total_productos; i++) {
        if (comparar_insensible(inventario[i].nombre, nombre) == 0) return i;
    }
    return -1;
}

void alta_producto() {
    if (total_productos >= MAX_PRODUCTOS) {
        printf("Capacidad máxima de inventario alcanzada.\n");
        return;
    }
    Producto p;
    printf("Nombre del producto: ");
    getchar();
    fgets(p.nombre, NOMBRE_LEN, stdin);
    p.nombre[strcspn(p.nombre, "\n")] = '\0';
    printf("Cantidad: ");
    if (scanf("%d", &p.cantidad) != 1 || p.cantidad < 0) {
        printf("Cantidad inválida.\n");
        return;
    }
    printf("Precio: ");
    if (scanf("%f", &p.precio) != 1 || p.precio < 0) {
        printf("Precio inválido.\n");
        return;
    }
    // Verificar si ya existe (actualiza cantidad)
    int idx = buscar_indice_por_nombre(p.nombre);
    if (idx >= 0) {
        inventario[idx].cantidad += p.cantidad;
        inventario[idx].precio = p.precio;
        printf("Producto existente actualizado.\n");
    } else {
        strncpy(inventario[total_productos].nombre, p.nombre, NOMBRE_LEN);
        inventario[total_productos].cantidad = p.cantidad;
        inventario[total_productos].precio = p.precio;
        total_productos++;
        printf("Producto agregado.\n");
    }
    guardar_inventario();
}

void modificar_producto() {
    char nombre[NOMBRE_LEN];
    printf("Ingrese el nombre del producto a modificar: ");
    getchar();
    fgets(nombre, NOMBRE_LEN, stdin);
    nombre[strcspn(nombre, "\n")] = '\0';
    int idx = buscar_indice_por_nombre(nombre);
    if (idx < 0) {
        printf("Producto no encontrado.\n");
        return;
    }
    printf("Nuevo nombre (dejar vacío para no cambiar): ");
    char nuevo_nombre[NOMBRE_LEN];
    fgets(nuevo_nombre, NOMBRE_LEN, stdin);
    nuevo_nombre[strcspn(nuevo_nombre, "\n")] = '\0';
    if (strlen(nuevo_nombre) > 0) strncpy(inventario[idx].nombre, nuevo_nombre, NOMBRE_LEN);

    printf("Nueva cantidad (negativo para no cambiar): ");
    int cant;
    if (scanf("%d", &cant) == 1 && cant >= 0) inventario[idx].cantidad = cant;
    printf("Nuevo precio (negativo para no cambiar): ");
    float pr;
    if (scanf("%f", &pr) == 1 && pr >= 0) inventario[idx].precio = pr;
    guardar_inventario();
    printf("Producto modificado.\n");
}

void eliminar_producto() {
    char nombre[NOMBRE_LEN];
    printf("Ingrese el nombre del producto a eliminar: ");
    getchar();
    fgets(nombre, NOMBRE_LEN, stdin);
    nombre[strcspn(nombre, "\n")] = '\0';
    int idx = buscar_indice_por_nombre(nombre);
    if (idx < 0) {
        printf("Producto no encontrado.\n");
        return;
    }
    for (int i = idx; i < total_productos -1; i++) inventario[i] = inventario[i+1];
    total_productos--;
    guardar_inventario();
    printf("Producto eliminado.\n");
}

void registrar_venta() {
    char nombre[NOMBRE_LEN];
    int cantidad;
    printf("\n--- REGISTRAR VENTA ---\n");
    printf("Nombre del producto: ");
    getchar();
    fgets(nombre, NOMBRE_LEN, stdin);
    nombre[strcspn(nombre, "\n")] = '\0';
    int idx = buscar_indice_por_nombre(nombre);
    if (idx < 0) {
        printf("Producto no encontrado.\n");
        return;
    }
    printf("Cantidad a vender: ");
    if (scanf("%d", &cantidad) != 1 || cantidad <= 0) {
        printf("Cantidad inválida.\n");
        return;
    }
    if (cantidad > inventario[idx].cantidad) {
        printf("No hay suficiente stock. Stock actual: %d\n", inventario[idx].cantidad);
        return;
    }
    float total = cantidad * inventario[idx].precio;
    inventario[idx].cantidad -= cantidad;
    guardar_inventario();

    FILE *f = fopen(VENTAS_FILE, "a");
    if (f) {
        fprintf(f, "%s;%d;%.2f\n", inventario[idx].nombre, cantidad, total);
        fclose(f);
    } else {
        printf("Error al guardar venta.\n");
    }
    printf("Venta registrada. Total: %.2f\n", total);
}

void evaluar_eficiencia() {
    FILE *f = fopen(VENTAS_FILE, "r");
    if (!f) {
        printf("No hay registros de ventas para evaluar.\n");
        return;
    }
    int total_ventas = 0;
    int total_items = 0;
    float monto_total = 0.0f;
    while (!feof(f)) {
        char nombre[NOMBRE_LEN];
        int cant;
        float mont;
        if (fscanf(f, "%49[^;];%d;%f\n", nombre, &cant, &mont) == 3) {
            total_ventas++;
            total_items += cant;
            monto_total += mont;
        } else break;
    }
    fclose(f);
    if (total_ventas == 0) {
        printf("No hay ventas registradas.\n");
        return;
    }
    printf("\n--- EVALUACIÓN DE EFICIENCIA DE VENTAS ---\n");
    printf("Número de transacciones: %d\n", total_ventas);
    printf("Total de artículos vendidos: %d\n", total_items);
    printf("Monto total vendido: %.2f\n", monto_total);
    printf("Artículos promedio por venta: %.2f\n", (float)total_items / total_ventas);
    printf("Venta promedio (importe): %.2f\n", monto_total / total_ventas);
}

void menu_gestionar_productos() {
    int op;
    do {
        printf("\n--- GESTIONAR PRODUCTOS ---\n");
        printf("1. Alta de producto\n");
        printf("2. Modificar producto\n");
        printf("3. Consultar productos\n");
        printf("4. Eliminar producto\n");
        printf("5. Regresar\n");
        printf("Seleccione una opción: ");
        scanf("%d", &op);
        switch (op) {
            case 1: alta_producto(); break;
            case 2: modificar_producto(); break;
            case 3: listar_productos(); break;
            case 4: eliminar_producto(); break;
            case 5: break;
            default: printf("Opción inválida.\n");
        }
    } while (op != 5);
}

int main() {
    cargar_inventario();
    int opcion;
    do {
        printf("\n--- INVENTARIO OJITOS BONITOS ---\n");
        printf("1. Gestionar productos\n");
        printf("2. Registrar venta\n");
        printf("3. Evaluar eficiencia\n");
        printf("4. Salir\n");
        printf("Seleccione una opción: ");
        if (scanf("%d", &opcion) != 1) { printf("Entrada inválida.\n"); break; }
        switch (opcion) {
            case 1: menu_gestionar_productos(); break;
            case 2: registrar_venta(); break;
            case 3: evaluar_eficiencia(); break;
            case 4: printf("Saliendo...\n"); break;
            default: printf("Opción inválida.\n");
        }
    } while (opcion != 4);
    return 0;
}
