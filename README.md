#Sevidor TCP
Servidor TCP programado en C (NICOLAS BARRIENTOS - FAVA MAXIMILIANO)

#Objetivos del Trabajo Práctico:
  Desarrollar un servidor web que sea capaz de manejar solicitudes HTTP GET y HEAD.
  Implementar una respuesta adecuada en caso de recibir una orden diferente a GET o HEAD.
  Mostrar por pantalla todo lo recibido del cliente para fines de depuración y seguimiento del funcionamiento del servidor.

#Primeras decisiones de programación:
  Empezamos eligiendo el protocolo TCP de la familia TCP/IP para definir la comunicación entre el Servidor web y los clientes. Elegimos TCP porque es un protocolo orientado a la conexión para garantizar la entrega de los archivos solicitados y uno de los protocolos más usados mundialmente.

En el archivo server.c se define la estructura básica del servidor, su objetivo principal es manejar las solicitudes de los clientes de manera concurrente y proporcionar respuestas adecuadas según lo solicitado.

El servidor está configurado para escuchar en el puerto 8000 y puede manejar múltiples conexiones de clientes simultáneamente. Utiliza un enfoque basado en procesos para manejar la concurrencia: el proceso principal (Proceso Padre) está a la escucha de nuevas conexiones. Una vez un cliente se conecte al servidor; el servidor aceptará la conexión y delegará cada conexión a un proceso hijo para atender al cliente utilizando fork() y el hijo usara execv() para realizar la repuesta al cliente dependiendo de su solicitud.

En las llamadas a execv() que utilizara el proceso hijo se encuentran disponibles dos tipos de archivos .c:
requestget.c: Este archivo llamado con execv() contiene lo necesario para responder a una solicitud de tipo GET, devuelve el archivo solicitado al cliente ya sea una página HTML o una imagen JPG.
Está diseñado para diferenciar el tipo de extensión que el cliente requiera en la solicitud, ya sea HTML o JPG. Si alguna extensión es diferente a las ya mencionadas serán rechazadas. 
requesthead.c: Este archivo llamado con execv() contiene lo necesario para responder a una solicitud de tipo HEAD, devuelve la cabecera del archivo solicitado al cliente ya sea una página HTML o una imagen JPG.
Está diseñado para diferenciar el tipo de extensión que el cliente requiera en la solicitud, ya sea HTML o JPG. Si alguna extensión es diferente a las ya mencionadas serán rechazadas.
