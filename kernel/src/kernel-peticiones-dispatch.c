#include "kernel-peticiones-dispatch.h"

// Variables globales:
// COMMAND diccionario_peticiones[] = {
//     {IO, peticion_IO},
//     {FIN_QUANTUM, peticion_fin_quantum},
//     {WAIT, peticion_wait},
//     {SIGNAL, peticion_signal},
//     {FIN_EJECUCION, peticion_fin_ejecucion},
// }

// Funciones que ejecutan las peticiones:

// void* peticion_IO(t_pcb *pcb) {
//     char *interface_name = recibir_interface_name();
//     interface_io *interface = get_interface_from_dict(interface_name);
//     tipo_operacion operacion = recibir_operacion(); 

//     // Verificamos que la interfaz exista:
//     if (consulta_existencia_interfaz(interface)) {
//         move_pcb_to_exit();

//         return NULL; // Salimos de la funcion
//     }

//     // Verificamos que la operacion sea valida:
//     if(consulta_interfaz_para_aceptacion_de_operacion(interface, operacion)) {
        

//     } else
//         move_pcb_to_exit();
// }


// void peticion_fin_quantum() {
//     // Hay que loggear que se termino el quantum
//     // Cambio de contexto
//     // Hay que loggear que se cambio de contexto
//     // Se debe enviar el pcb a la cola de ready -> cambiar_ready_a_ready(pcb)
// }