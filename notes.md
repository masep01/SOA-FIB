# Notes for E2
maximo 10 PCBs
hay que añadir:
+ Free queue -> inicializar a vacia (hay funcion que lo hace) +  añadir todos los tasks_unions en: init_sched()
+ Ready queue -> inicializar a vacia (hay funcion que lo hace) en: init_sched()
+ con struct list_head

## struct list_head
Para crear listas/colas dinámicas
+ Ready queue -> list_head
+ Free queue  -> 
+ Para encolar un elemento -> creo un list_head
+ Hacer una funcion: list_head_to_task_struct que dado un list head devuelva un taskStruct

## init_idle()
Proceso que se mete en CPU, cuando no hay ningun proceso que se pueda ejecutar
+ Para q no se apague la CPU.  
+ Lo creo en tiempo d boot y en modo sistema  
+ Lo tengo q preparar para q entre en la CPU pero q no salga a Usuario.  
+ Sera un `while(1)` en modo sistema.

### Como lo hago?
En su pila de sistema no habra Ctx SW o Ctx Hw, porque no volvera a Usuario.
+ Pongo abajo de la pila: [epb][@ret]
+ Y hago que kernel_esp apunte a %ebp
+ Cojo un task_struct d la cola de free, inicializar la pila de sistema asi.
+ idle es un proceso que nunca ira a cola de Ready

## init_task1()
Primer proceso de Usuario
1. Definimos que pinta tiene la TP en ZeOS
2. Esta en `mm_address.h`
3. Hay una funcion que lo hace -> `set_user_pages()`
4. Hacer:
```
cr3 -> TP
tss.esp0 -> system_stack
MSR[0x175] -> system_stack
```

5. Hay que poner task1 en ready, con la funcion -> trampolin()  

## Como encuentro la pila de sistema del proceso actual?
current() devuelve -> puntero a task_struct
```
((union task_union*)p) -> stack
```

<br>

## Fork
Cuando me dicen que asigne un directorio al proceso -> Quiere decir que asigno un directorio y un trozo de la TP.

## Todo
+ ~~Ready queue~~
+ ~~Free queue~~
+ ~~Inicializar estas estructuras~~
+ ~~crear idle, task1~~
+ ~~`task_switch` ---> `inner_task_switch`~~
    - ~~Se tiene que implementar un `task_switch` que llame a ~~`inner_task_switch` y que hace: ~~ 
        1. ~~ Guardar en la pila `ebx`, `esi` y `edi`~~
        2. ~~Llamar a `inner_task_switch`~~
   ~~ - Un trozo va `sched.c` y el ot~~

+ `sys_fork`
+ `scheduler`
    - RoundRobin (RR) ---> Quantum (dentro de `task_struct`)
    - Dentro de `clock_routine`: se llama a `schedule()`
        1. `schedule()` llama a `update_sched_data()` (decrementa tics restantes del proceso para salir d la CPU)
        2. implementar `needs_sched()`: mira si se ha expirado el quantum del proceso actual.
            - Devuelve 0 si aun tiene
            - Devuelve 1 si hay que hecharlo
        3. implementar `update_process_state()`: cambia de estado a un proceso, tiene 2 parametros, el proceso y la cola donde va a ir.

        ```C
        schedule(){
            update_sched_data();
            if(needs_sched()){
                update_process_state();
                sched.next();
            }
        }
        ```
+ `block`: bloquea el proceso
+ `unblock` desbloquea el proceso