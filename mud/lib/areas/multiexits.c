
inherit "/lib/room";

void setup() {
     set_light(40);
     set_short(mud_name()+": El Vacío");
     set_long("Éste es el lugar al que vienes cuando ha habido un fallo "
              "en el lugar en el que estabas la última vez que jugaste, "
              "y te quedas sin ningún lugar al que ir. "
              "La razón se deberá muy probablemente "
              "a algún fallo involuntario de programación o a un error del sistema. "
              "Utiliza el comando 'emergencia' para avisar de lo que ha pasado "
              "(indica dónde te encontrabas la última vez si lo recuerdas) y que así "
              "quede constancia de lo que ha sucedido para que se pueda arreglar, "
              "y luego sal de este lugar utilizando las salidas a tu disposición.\n");

     add_exit("ciudad", "/d/ciudad_capital/barrio_antiguo/rooms/calzada02.c", "standard");
}
