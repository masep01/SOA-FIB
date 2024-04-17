#include <libc.h>

char buff[64];

int pid;

char* quijote = "\nEn un lugar de la Mancha2, de cuyo nombre no quiero acordarme3, no ha mucho tiempo que vivía un hidalgo de los de lanza en astillero, adarga antigua, rocín flaco y galgo corredor4. Una olla de algo más vaca que carnero, salpicón las más noches5, duelos y quebrantos los sábados6, lantejas los viernes7, algún palomino de añadidura los domingos8, consumían las tres partes de su hacienda9. El resto della concluían sayo de velarte10, calzas de velludo para las fiestas, con sus pantuflos de lo mesmo11, y los días de entresemana se honraba con su vellorí de lo más fino12. Tenía en su casa una ama que pasaba de los cuarenta y una sobrina que no llegaba a los veinte, y un mozo de campo y plaza que así ensillaba el rocín como tomaba la podadera13. Frisaba la edad de nuestro hidalgo con los cincuenta años14. Era de complexión recia, seco de carnes, enjuto de rostro15, gran madrugador y amigo de la caza. Quieren decir que tenía el sobrenombre de «Quijada», o «Quesada», que en esto hay alguna diferencia en los autores que deste caso escriben, aunque por conjeturas verisímilesII se deja entender que se llamaba «Quijana»III, 16. Pero esto importa poco a nuestro cuento: basta que en la narración dél no se salga un punto de la verdad.Es, pues, de saber que este sobredicho hidalgo, los ratos que estaba ocioso —que eran los más del año—, se daba a leer libros de caballerías, con tanta afición y gusto, que olvidó casi de todo punto el ejercicio de la caza y aun la administración de su hacienda; y llegó a tanto su curiosidad y desatino en esto17, que vendió muchas hanegas de tierra de sembradura para comprar libros de caballerías en queIV leer18, y, así, llevó a su casa todos cuantos pudo haber dellos; y, de todos, ningunos le parecían tan bienV como los que compuso el famoso Feliciano de Silva19, porque la claridad de su prosa y aquellas entricadas razones suyas le parecían de perlas, y más cuando llegaba a leer aquellos requiebros y cartas de desafíos20, donde en muchas partes hallaba escrito: «La razón de la sinrazón que a mi razón se hace, de tal manera mi razón enflaquece, que con razón me quejo de la vuestra fermosura»21. Y también cuando leía: «Los altos cielos que de vuestra divinidad divinamente con las estrellas os fortifican y os hacen merecedora del merecimiento que merece la vuestra grandeza...»22Con estas razones perdía el pobre caballero el juicio, y desvelábase por entenderlas y desentrañarles el sentido, que no se lo sacara ni las entendiera el mesmo Aristóteles, si resucitara para solo ello. No estaba muy bien con las heridas que don Belianís daba y recebía, porque se imaginaba que, por grandes maestros que le hubiesen curado, no dejaría de tener el rostro y todo el cuerpo lleno de cicatrices y señales23. Pero, con todo, alababa en su autor aquel acabar su libro con la promesa de aquella inacabable aventura, y muchas veces le vino deseo de tomar la pluma y dalle fin al pie de la letra como allí se promete24; y sin duda alguna lo hiciera, y aun saliera con ello25, si otros mayores y continuos pensamientos no se lo estorbaran. Tuvo muchas veces competencia con el cura de su lugar —que era hombre docto, graduado en Cigüenza—26 sobre cuál había sido mejor caballero: Palmerín de Ingalaterra o Amadís de Gaula27; mas maese Nicolás, barbero del mesmo pueblo28, decía que ninguno llegaba al Caballero del Febo, y que si alguno se le podía comparar era don Galaor, hermano de Amadís de Gaula, porque tenía muy acomodada condición para todo, que no era caballero melindroso, ni tan llorón como su hermano, y que en lo de la valentía no le iba en zaga29.En resolución, él se enfrascó tanto en su letura, que se le pasaban las noches leyendo de claro en claro30, y los días de turbio en turbio; y así, del poco dormir y del mucho leer, se le secó el celebro de manera que vino a perder el juicio31. Llenósele la fantasía de todo aquello que leía en los libros, así de encantamentos como de pendencias, batallas, desafíos, heridas, requiebros, amores, tormentas y disparates imposibles; y asentósele de tal modo en la imaginación que era verdad toda aquella máquina de aquellas soñadas invencionesVI que leía32, que para él no había otra historia más cierta en el mundo33. Decía él que el Cid Ruy Díaz había sido muy buen caballero, pero que no tenía que ver con el Caballero de la Ardiente Espada, que de solo un revés había partido por medio dos fieros y descomunales gigantes34. Mejor estaba con Bernardo del Carpio, porque en Roncesvalles había muerto a Roldán, el encantado35, valiéndose de la industria de Hércules, cuando ahogó a AnteoVII, el hijo de la Tierra, entre los brazos36. Decía mucho bien del gigante Morgante, porque, con ser de aquella generación gigantea, que todos son soberbios y descomedidos, él solo era afable y bien criado37. Pero, sobre todos, estaba bien con Reinaldos de Montalbán, y más cuando le veía salir de su castillo y robar cuantos topaba, y cuando en allende robó aquel ídolo de Mahoma que era todo de oro, según dice su historia38. Diera él, por dar una mano de coces al traidor de Galalón39, al ama que tenía, y aun a su sobrina de añadidura.\nEn un lugar de la Mancha2, de cuyo nombre no quiero acordarme3, no ha mucho tiempo que vivía un hidalgo de los de lanza en astillero, adarga antigua, rocín flaco y galgo corredor4. Una olla de algo más vaca que carnero, salpicón las más noches5, duelos y quebrantos los sábados6, lantejas los viernes7, algún palomino de añadidura los domingos8, consumían las tres partes de su hacienda9. El resto della concluían sayo de velarte10, calzas de velludo para las fiestas, con sus pantuflos de lo mesmo11, y los días de entresemana se honraba con su vellorí de lo más fino12. Tenía en su casa una ama que pasaba de los cuarenta y una sobrina que no llegaba a los veinte, y un mozo de campo y plaza que así ensillaba el rocín como tomaba la podadera13. Frisaba la edad de nuestro hidalgo con los cincuenta años14. Era de complexión recia, seco de carnes, enjuto de rostro15, gran madrugador y amigo de la caza. Quieren decir que tenía el sobrenombre de «Quijada», o «Quesada», que en esto hay alguna diferencia en los autores que deste caso escriben, aunque por conjeturas verisímilesII se deja entender que se llamaba «Quijana»III, 16. Pero esto importa poco a nuestro cuento: basta que en la narración dél no se salga un punto de la verdad.Es, pues, de saber que este sobredicho hidalgo, los ratos que estaba ocioso —que eran los más del año—, se daba a leer libros de caballerías, con tanta afición y gusto, que olvidó casi de todo punto el ejercicio de la caza y aun la administración de su hacienda; y llegó a tanto su curiosidad y desatino en esto17, que vendió muchas hanegas de tierra de sembradura para comprar libros de caballerías en queIV leer18, y, así, llevó a su casa todos cuantos pudo haber dellos; y, de todos, ningunos le parecían tan bienV como los que compuso el famoso Feliciano de Silva19, porque la claridad de su prosa y aquellas entricadas razones suyas le parecían de perlas, y más cuando llegaba a leer aquellos requiebros y cartas de desafíos20, donde en muchas partes hallaba escrito: «La razón de la sinrazón que a mi razón se hace, de tal manera mi razón enflaquece, que con razón me quejo de la vuestra fermosura»21. Y también cuando leía: «Los altos cielos que de vuestra divinidad divinamente con las estrellas os fortifican y os hacen merecedora del merecimiento que merece la vuestra grandeza...»22Con estas razones perdía el pobre caballero el juicio, y desvelábase por entenderlas y desentrañarles el sentido, que no se lo sacara ni las entendiera el mesmo Aristóteles, si resucitara para solo ello. No estaba muy bien con las heridas que don Belianís daba y recebía, porque se imaginaba que, por grandes maestros que le hubiesen curado, no dejaría de tener el rostro y todo el cuerpo lleno de cicatrices y señales23. Pero, con todo, alababa en su autor aquel acabar su libro con la promesa de aquella inacabable aventura, y muchas veces le vino deseo de tomar la pluma y dalle fin al pie de la letra como allí se promete24; y sin duda alguna lo hiciera, y aun saliera con ello25, si otros mayores y continuos pensamientos no se lo estorbaran. Tuvo muchas veces competencia con el cura de su lugar —que era hombre docto, graduado en Cigüenza—26 sobre cuál había sido mejor caballero: Palmerín de Ingalaterra o Amadís de Gaula27; mas maese Nicolás, barbero del mesmo pueblo28, decía que ninguno llegaba al Caballero del Febo, y que si alguno se le podía comparar era don Galaor, hermano de Amadís de Gaula, porque tenía muy acomodada condición para todo, que no era caballero melindroso, ni tan llorón como su hermano, y que en lo de la valentía no le iba en zaga29.En resolución, él se enfrascó tanto en su letura, que se le pasaban las noches leyendo de claro en claro30, y los días de turbio en turbio; y así, del poco dormir y del mucho leer, se le secó el celebro de manera que vino a perder el juicio31. Llenósele la fantasía de todo aquello que leía en los libros, así de encantamentos como de pendencias, batallas, desafíos, heridas, requiebros, amores, tormentas y disparates imposibles; y asentósele de tal modo en la imaginación que era verdad toda aquella máquina de aquellas soñadas invencionesVI que leía32, que para él no había otra historia más cierta en el mundo33. Decía él que el Cid Ruy Díaz había sido muy buen caballero, pero que no tenía que ver con el Caballero de la Ardiente Espada, que de solo un revés había partido por medio dos fieros y descomunales gigantes34. Mejor estaba con Bernardo del Carpio, porque en Roncesvalles había muerto a Roldán, el encantado35, valiéndose de la industria de Hércules, cuando ahogó a AnteoVII, el hijo de la Tierra, entre los brazos36. Decía mucho bien del gigante Morgante, porque, con ser de aquella generación gigantea, que todos son soberbios y descomedidos, él solo era afable y bien criado37. Pero, sobre todos, estaba bien con Reinaldos de Montalbán, y más cuando le veía salir de su castillo y robar cuantos topaba, y cuando en allende robó aquel ídolo de Mahoma que era todo de oro, según dice su historia38. Diera él, por dar una mano de coces al traidor de Galalón39, al ama que tenía, y aun a su sobrina de añadidura.\n";

void show_current_time(){
    itoa(gettime(), buff);
    if(write(1, " Current time: ", strlen("Current time: ")) < 0) perror();
    if(write(1, buff, strlen(buff)) < 0) perror();
    if(write(1, "\n", 1) < 0) perror();
}

void test_write(){
  char* banner_W = "\n\n[!] Testing write syscall.\n";
  if(write(1, banner_W, strlen(banner_W)) < 0) perror();

  /* Case 1: Bad file number */
  if(write(-1, "Test1", strlen("Test1")) < 0) perror();

  /* Case 2: Bad address */
  char* b;
  if(write(1, b, strlen("Test2.1")) < 0) perror();        // Test 2.1: NULL buffer
  if(write(1, 0x1234, strlen("Test2.2")) < 0) perror();   // Test 2.2: Invalid address

  /* Case 3: Invalid argument */
  if(write(1, "Test3", -1) < 0) perror();
}

void test_gettime(){
  char* banner_gt = "\n[!] Testing getTime syscall.\n";
  if(write(1, banner_gt, strlen(banner_gt)) < 0) perror();
  show_current_time();
  while(gettime()<1000){}
  show_current_time();
}

void test_pfault(){
  if(write(1, "\n[!] Testing Page Fault exception\n", strlen("\n[!] Testing Page Fault exception\n")) < 0) perror();
  char* p = 0;
  *p = 'x';
}

void test_getpid(){
  itoa(getpid(), buff);
  if(write(1, "\n[!] Current PID: ", strlen("\n[!] Current PID: ")) < 0) perror();
  if(write(1, buff, strlen(buff)) < 0) perror();
  if(write(1, "\n", strlen("\n")) < 0) perror();
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
    /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

    /* Testing Syscall: Write */
    //test_write();

    /* Testing Syscall: getTime() */
    //test_gettime();

    /* Testing Page fault exception */
    // test_pfault();
    
    /* Testing getpid */
    //test_getpid();

    int pid = fork();
    itoa(pid, buff);
    
    if(pid > 0){
      write(1, "\nSoy el padre.\n", sizeof("\nSoy el padre.\n"));
      test_getpid();
      exit();
      while(1) write(1, "\nSigo en padre.\n", sizeof("\nSigo en padre.\n"));

    } else {
      write(1, "\nSoy el hijo.\n", sizeof("\nSoy el hijo.\n"));
      test_getpid();
      exit();
      while(1) write(1, "\nSigo en hijo.\n", sizeof("\nSigo en hijo.\n"));
    }

    // Fails when both father and son are alive and they are switching always. Eventually we recieve a PAGEFAULT.
  while(1) { }
}
