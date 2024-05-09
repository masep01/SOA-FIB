# COW
## Cambios en `sys_fork`
1) marco [bit escritura] = 0 de PAGINAS FISICAS DE DATOS
2) pongo a 0 las referencias de PAGINAS FISICAS DE DATOS

## Cambios en `pagefault_exception`
1) Miro `phys_mem` para saber `#refs`
    - Si `#refs` = 1 --> la pagina que solicita sera suya
    - Si `#refs` > 1 --> creo copia, decremento referencias y le cambio el mapeo para que pueda escribir

## Cambios en `mm.c`
Hay que cambiar `phys_mem`:  
- **Ahora**: me sirve para saber si una pagina esta ocupada.   
Para cada posicion me dice si `0` libre o `1` usada.  

- **CAMBIO**: Cada posición --> `#refs` a esa pagina. 
    - `#refs` = 0 --> LIBRE
    - `#refs` > 1 --> COMPARTIDA con alguien