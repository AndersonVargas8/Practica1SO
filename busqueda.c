#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>



struct typeViaje
{
    int idOrigen;
    int idDestino;
    int hora;
    float meanTravelTime;
    float stDevTravelTime;
    float geoMeanTravelTime;
    float geoStDesTravelTime;
    int nextIdOrigen;
};
struct typeIndice
{
    int posicion;
};
struct inType{
    int origen, destino, hora;
};
int buscarIndice(int origenId)
{
    FILE *archivo;
    archivo = fopen("indices.dat", "rb");
    int posicion = 0;
    if (archivo == NULL)
        exit(-1);
    fseek(archivo, sizeof(struct typeIndice) * origenId, SEEK_SET);
    struct typeIndice indice;
    fread(&indice, sizeof(struct typeIndice), 1, archivo);
    fclose(archivo);
    return indice.posicion;
}
float busqueda(int origenId, int destinoId, int hora)
{
    int posicion = buscarIndice(origenId);
    float mediaViaje = -1;
    if(posicion==-1){
        return mediaViaje;
    }
    FILE *archivo;
    archivo = fopen("datos.dat", "rb");
    if (archivo == NULL)
        exit(-1);
    fseek(archivo, posicion * sizeof(struct typeViaje), SEEK_SET);
    struct typeViaje viaje;
    fread(&viaje, sizeof(struct typeViaje), 1, archivo);

    fseek(archivo, posicion * sizeof(struct typeViaje), SEEK_SET);

    while (viaje.idDestino != destinoId || viaje.hora != hora)
    {
        posicion = viaje.nextIdOrigen;
        fseek(archivo, posicion * sizeof(struct typeViaje), SEEK_SET);
        fread(&viaje, sizeof(struct typeViaje), 1, archivo);
        if (viaje.nextIdOrigen == -1)
            break;
    }
    if (viaje.idDestino == destinoId && viaje.hora == hora)
    {
        mediaViaje = viaje.meanTravelTime;
    }
    fclose(archivo);
    return mediaViaje;
}
int main(){
    float media;
    struct inType *ingreso;
    ingreso = malloc(sizeof(struct inType));
    int fd;

    fd=open("/tmp/mi_fifo",O_RDONLY);

    read(fd,ingreso,sizeof(struct inType));

    close(fd);

    clock_t time1 = clock();

    media = busqueda(ingreso->origen, ingreso->destino, ingreso->hora);


    printf(" PID BUSQUEDA %d\n",getpid());

//ESCRITURA

    fd=open("/tmp/mi_fifo",O_WRONLY);    
    
    write(fd,&media,sizeof(float));

    clock_t time2 = clock();

    double time = (double)(time2-time1)/CLOCKS_PER_SEC;
    printf("Tiempo de búsqueda:%lf Segundos\n",time);
    close(fd);
    free(ingreso);
}
