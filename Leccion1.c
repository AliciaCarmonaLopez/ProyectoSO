#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int main(int argc, char *argv[])
{
    // ESTABLECIMIENTO DE LA CONEXIN SERVIDOR-CLIENTE
    int sock_conn, sock_listen, ret;
    struct sockaddr_in serv_adr;
    char peticion [512];
    char respuesta[512];
    //abrir el socket de escucha
    if((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) <0)
    {
        printf("Error creant el socket");
    }
    memset(&serv_adr, 0, sizeof(serv_adr)); //inicialitza a 0 el serv_adr
    serv_adr.sin_family = AF_INET;
    // INICIO DE LA ESTRUCTURA DE DATOS
    //asigna la ip del computador que ejecute el programa
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    // asigna el puerto a utilizar
    serv_adr.sin_port = htons(9080);
    // ASOCIAR AL SOCKET DE ESCUCHA LA ESTRUCTURA DE DATOS
    if(bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr))<0)
        printf("Error al bind");
    
    if(listen(sock_listen, 3) < 0)
        printf("Error al listen");//error si la cola de connexiones esperando a ser atendidas es mayor de 3
    
    
    // INICIO DEL BUCLE DEL SERVIDOR
    int i;
    for(i = 0; i<5; i++) //atiende solo 5 peticiones pero suele ser infinito
    {
        printf("escuchando\n");
        // el servidor esta parado hasta que alguien se conecte, y cuando pasa genera un socket diferente (de conexión)
        sock_conn = accept(sock_listen, NULL, NULL); //socket a través del cual se conecta con la máquina
        printf("He recibido conexion\n");
       
        //recibe la peticion y la guarda en el vector peticion
        ret = read(sock_conn, peticion, sizeof(peticion)); //read lee la peticion y devuelve el numero de bytes leidos
        printf("Recibido\n");
       
        //se añade una marca de fin de string en peticion para que no lea lo que hay mas allá del mensaje
        peticion[ret] = '\0';
       
        // INICIO DE LA OPERACIN DE DETERMINAR QUÉ ME PIDEN
        //Escribimos el mensaje peticion
        printf("Petición: %s", peticion);
        //separar el tipo de peticion del contenido
        char *p = strtok(peticion, "/");
        int codigo = atoi(p); // asigna el tipo a codigo
        p = strtok(NULL, "/"); // asigna el contenido a p
        char nombre[20];
        strcpy(nombre, p); //copia el contenido del puntero a la variable
        printf("Codigo: %d, Nombre: %s", codigo, nombre);
       
        // DETERMINA QUE ME PIDEN Y PREPARA LA RESPUESTA
        if(codigo == 1) // me piden la largura del nombre
            sprintf(respuesta, "%d", strlen(nombre));
        else if(codigo == 2) // me piden si el nombre es bonito
        {
            if((nombre[0] == 'M')||(nombre[0] == 'S'))
                strcpy(respuesta, "SI");
            else
                strcpy(respuesta, "NO");
        }
        else // me piden determinar si es alto
        {
            p = strtok(NULL, "/");
            float altura = atof(p);
            if(altura > 1.70)
            {
                sprintf(respuesta, "%s : eres alto", nombre);
            }
            else
               sprintf(respuesta, "%s : eres bajo", nombre);
        }
        printf("Respuesta: %s\n", respuesta);
       
        // ENVIAR LA RESPUESTA
        write(sock_conn, respuesta, strlen(respuesta));
       
        // CERRAR LA CONEXIÓN
        close(sock_conn);
         
        //VUELVE A EMPEZAR EL BUCLE
    }
    
    return 0;
}