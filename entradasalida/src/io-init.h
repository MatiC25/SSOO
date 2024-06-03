#ifndef IO_INIT_H
#define IO_INIT_H

// Incluye las bibliotecas necesarias:
#include <utils/estructuras_compartidas.h>
#include <utils/logger.h>
#include <commons/config.h>
#include <utils/shared.h>
#include "io.h"
// Incluye las estructuras necesarias:
#include "io-config.h"
#include "io-estructuras.h"



// Funciones para inicializar:
void inicializar_interfaz(char *name_interfaz, char *config_path,t_interfaz* interfaz, t_config_io* config_io);
//t_config_io *cargar_configuraciones(char *config_path);
void cargar_configuraciones(char *config_path, t_interfaz* interfaz, t_config_io* config_io);
// Funciones para cargar configuraciones correspondientes a cada tipo de interfaz:
void cargar_configuraciones_generica(t_config *config, t_config_io* config_io);
void cargar_configuraciones_std(tipo_interfaz tipo, t_config *config, t_config_io* config_io);
void cargar_configuraciones_dialfs(t_config *config, t_config_io* config_io);

// Funciones para obtener el tipo de interfaz:
void get_tipo_interfaz_with_config(t_config *config , t_interfaz* interfaz);
tipo_interfaz get_tipo_interfaz(char *tipo);

#endif // IO_INIT_H