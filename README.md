SysOp
=====

[Gestion File Di Log]

Per quanto riguarda la registrazione di tutte le attività (errori e messaggi di informazione), in tutte e tre le utilities sviluppate facciamo uso del demone di sistema "syslog", il quale permette di salvare dei messaggi di log aventi una priorità stabilità al momento della chiamata, salvandoli nel percorso di default /var/log/syslog.
Questa scelta è dovuta al fatto che, con questo demone di sistema messo a disposizione da unix, i messaggi contengono la data e l'ora di quando sono stati generati, l'user id e il PID del processo che ha chimato la system call.
Inoltre è possibile differenziare messaggi di errore da messaggi di warning, informazione oppure di critical conditions.

Mkbkp [ PERANTONI ]
====

Le funzionalità richieste per l'utility mkbkp sono le seguenti:

- Creare un'archivio per salvare file e directory che vengono passati come parametri, è previsto l'utilizzo di diversi flag per identificare le diverse funzionalità dell'applicazione:
	* -f <archivio> viene utilizzato in concomitanza con le altre opzioni che seguono. Permette di speficiare l'archivio che dovrà essere creato, estratto o ispezionato.

	* -c permette di creare un archivio. Devono essere passati come parametri il nome dell'archivio da creare (con relativa estensione, ad es: file.bkp); se l'estensione non viene specificata verrà comunque aggiunta automaticamente. Viene assunto che durante la creazione dell'archivio lo stesso venga creato nella cartella corrente di lavoro, cioè quella da dove viene eseguita l'utlity.

	* -x permette di estrarre un archivio nella directory corrente. Per utilizzare questa funzionalità oltre al flag -x deve essere utilizzato il flag -f per specificare il nome dell'archivio da estrarre.

	* -t permette di visualizzare i file contenuti all'interno di un archivio. Come per le opzioni precedenti deve essere utilizatto in coppia con il flag -f per specificare di quale archivio si vuole visualizzare il contenuto.

- Attraverso l'utilizzo della syscall getopt() vengono presi in input i flag utilizzati dall'utente (e relativi parametri), successivamente vengono effettuati diversi controlli sugli stessi:

	* All'attivazione tramite la syscall getopt() di ognuno dei flag messi a disposizione dall'utility vengono utilizzate delle variabili per poter riconoscere che un determinato flag è stato attivato.

	* In questo modo è possibile eseguire un controllo specifico sui parametri che sono stati inseriti dall'utente e visualizzare i relativi messaggi di errore.

- La creazione di un archivio viene gestita nel seguente modo:

	* Per prima cosa viene letta la directory corrente dalla quale è stata eseguita l'utility: è stato assunto che il backup venga creato in quella directory.

	* Successivamente l'utility inizia a leggere la cartella di cui fare il backup passata in inpute e vengono gestiti due casi diversi:

		* Se l'elemento corrente che viene letto è una cartella allora viene chiamata ricorsivamente la funzione di creazione del backup (passando come parametro la directory appena trovata) così, in questo modo, l'utility entra nella directory appena letta e può effettuare il backup al suo interno.

		* Se l'elemento corrente che viene letto è invece un file allora viene letto ed aggiunto al file di backup in questo modo:

			* Abbiamo implementato un separatore che identifica i singoli file e aiuta anche nella fase di estrazione dello stesso, consiste in:

				- FILE=<path assoluto del file>
    			  \nENDFILE	 

		* Se non ci sono più file l'esecuzione della funzione di creazione del backup termina, altrimenti vengono eseguite le medesime operazioni ricorsivamente fino a quando non sono state passate tutte le sottodirectory e tutti i file presenti all'interno della cartella presa in input.

- L'estrazione di un'archivio viene gestita nel seguente modo:

	* Viene effettuato un parsing del file di backup, ogni file viene identificato grazie alla stringa che viene scritta prima di esso e che contiene, come già anticipato, la path del file.

	* Appena viene riconosciuto un file tramite la path si riconosce dove dovrà essere estratto; in seguito viene identificato il contenuto del file che sarà contenuto tra il 'new line' che segue la path e l'inizio del file successivo.

	* Se invece che un file viene riconosciuta una directory durante il parsing del file di backup allora viene creata una directory secondo la path indicata all'interno del file di backup.

Equal [ ZEN ]
=====

Introduction:

L'idea iniziale era quella di implementare la equal nel seguente modo:

Nel caso in cui i due percorsi rappresentassero due files si effettuava un controllo incrociato, leggendo una porzione dal primo file e scorrendo tutto il secondo fino a trovarne una porzione uguale oppure fino a quando il secondo file fosse stato letto completamente. In questo modo saremmo riusciti ad ottenere correttamente le differenze tra i files fino a trovare una porzione comune.

Per quanto riguarda le cartelle avremmo utilizzato la funzione adoperata per i files nel caso ci fossero stati due files omonimi; riconoscendo inoltre se la directory del primo path fosse contenuta nella directory del secondo path o viceversa.

Our Solution:

L' utility implementata è molto simile a quella già offerta da unix: la diff. Tuttavia differisce per alcune scelte implementative.
Innanzitutto i percorsi passati per parametro devono essere compatibili: entrambi devono rappresentare files o directory. Non può quindi essere confrontato un file con una directory o viceversa.

Se i due percorsi passati per parametro rappresentano due files allora per prima cosa si guarda se i percorsi sono uguali; in caso che di percorsi uguali ovviamente non viene stampato a video alcuna differenza perchè i files sono uguali.
In caso contrario si procede ad una lettura per bytes del file: questa scelta è dovuta al fatto che i files che si desiderano comparare possono essere di qualsiasi tipo: testuali, media. archivi ecc.
Diversamente dalla utility "diff" i buffers che procedono man mano ad esaminare i files si muovono in parallelo: si procede alla lettura del primo blocco di bytes.
Se i buffer sono uguali si procede alla lettura del successivo blocco di bytes su entrambi i files (quindi senza stampare alcuna differenza a video), altrimenti vengono stampati entrambi i buffers a video differenziano con una '+' il buffer del primo file e con una '-' il buffer del secondo file.
L'intera procedura viene ripetuta finchè i files non vengono letti completamente.
Se i files hanno dimensione diversa si procederà a leggere la porzione di testo rimanente al file con dimensione maggiore.

Nel caso si tratti di due directories, la procedura è diversa.
Per prima cosa, vengono confrontati i due percorsi: se si tratta di percorsi uguali, anche le directory avranno lo stesso contenuto.
Altrimenti, vengono letti tutti i files presenti nella prima cartella: ciascuno di esso viene confrontato con tutti i files presenti nella seconda cartella.
Se si trovano files omonimi si applica la equal su entrambi i files; se invece la seconda cartella non contiene il file che si sta cercando, viene stampata in console il nome del file con una '+' ad indicare che la prima cartella contiene un file che non è presente nella seconda.
Se si trovano cartelle omonime si applica ricorsivamente la equal che analizza i files contenuti in esse.

	equal A B  mostra a video le differenze che ha il (la) file (directory) A rispetto al (alla) file (directory) B.
	equal A B  mostra a video le differenze che ha il (la) file (directory) B rispetto al (alla) file (directory) A.

Usage:

Per quanto riguarda i files è sufficiente passare i paths dei files typo. Es:
	equal /home/user/file1.c /home/user/folder/file2.c

Per quanto riguarda le cartelle è sufficiente passare i paths delle cartelle omettendo l'ultimo SLASH. Es:
	equal /home/user/folder1 /home/user/folder2

Examples:

	/home/roberto/Scrivania/folder1							/home/roberto/Scrivania/folder2
	├── file1.txt 															├── file1.txt
	│   																				├── file2.txt
	│   																				├── img1.png
	├── subfolder1															├── subfolder1
	│   └── file1.txt 													│
	└── subfolder2															├── subfolder2
	    └── file2.txt 															└── file2.txt   

	2 directories, 3 files											2 directories, 4 files

	In questo caso:

		/home/roberto/Scrivania/folder1/file1.txt e /home/roberto/Scrivania/folder2/file1.txt hanno contenuto diverso.
		/home/roberto/Scrivania/folder1/subfolder/file2.txt e /home/roberto/Scrivania/folder2/subfolder2/file2.txt sono uguali.


	/home/roberto/Scrivania/folder1/file1.txt

	Questo
	è
	il file1.txt
	in folder1
	fine


	/home/roberto/Scrivania/folder2/file1.txt

	Questo
	è
	il file1.txt
	in folder2
	fine


	equal /home/roberto/Scrivania/folder1/file1.txt /home/roberto/Scrivania/folder2/file1.txt

	  Differences between files: /home/roberto/Scrivania/folder1/file1.txt - /home/roberto/Scrivania/folder2/file1.txt
    + in folder1
    - in folder2



  equal /home/roberto/Scrivania/folder1 /home/roberto/Scrivania/folder2

	  Differences between files: /home/roberto/Scrivania/folder1/file1.txt and /home/roberto/Scrivania/folder2/file1.txt
	    + in folder1
	    - in folder2

	  Differences between folders: /home/roberto/Scrivania/folder1/subfolder1 and /home/roberto/Scrivania/folder2/subfolder1
	    + file1.txt
	  - img1.png
	  - file2.txt


Plive [ DE FRANCESCO ]
=====

[Introduzione]
Il comando plive e' un utility che mostra i processi in uso sulla macchina ordinati per utilizzo di cpu e filtrati da un numero arbitrario N che viene scelto dall'utente all'avvio. Inoltre l'utente puo' modificare l'intervallo di tempo in cui il programma aggiorna la lista.

[La nostra soluzione]
L'implementazione di un utility simile alla top ci ha fatto domandare innanzitutto in che modo potevamo ottenere dal S.O i processi attualmente attivi sulla macchina e questo ci ha portato a scoprire lo pseudo-filesystem contenuto nella cartella /proc/. Infatti dentro questa directory possiamo trovare delle sottocartelle che hanno per nome il PID dei processi in esecuzione sulla macchina, quindi ci rimaneva il problema di come calcolare la cpu effettivamente utilizzata dalla cpu trovare tutte le altre informazioni che ci servivano quali parent-id, nome del processo e qualcosa per calcolare la cpu utilizzata dal processo.
Abbiamo notato che dentro ogni cartella di /proc/ c'e un file chiamato "stat" che contiene praticamente tutte le proprieta' su quel determinato processo e con questo abbiamo avuto tutte le informazioni necesserie ad iniziare a programmare.
All'inizio abbiamo avuto qualche difficolta' a causa del linguaggio C a causa del suo essere a piu' basso livello dei linguaggi di porgrammazione a cui siamo abituati ma siamo riusciti a superarle tutte.
L'unica cosa che ancora adesso ci lascia un po' perplessi e' il calcolo della cpu utilizzata da un processo poiche' abbiamo provato le seguenti formule:

	((tempo del processo in kernel mode letto adesso + tempo del processo il user mode letto adesso)-(tempo del processo in kernel mode + tempo del processo il user mode))
	/(tempo totale trascorso letto adesso - tempo totale trascorso letto prima)

Ma il valore che veniva fuori ci sembrava troppo piccolo per sembrare reale, quindi abbiamo provato con la stessa foruma di prima moltiplicando il numeratore * 100 poiche' delle varie unita' di misura ma ancora ci sembrava troppo discostante dal valore finale. Infine abbiamo optato per:

	(tempo del processo in user mode letto adesso - tempo del processo in user mode letto prima) / 
		(il numero di secondi che impieghiamo per fare il refresh)

Questo ancora non e' affidabile quanto quello del comando top ma sembra quello che ci si avvicina di piu'.

[Uso]

	./plive -n <numero dei processi che vogliamo vedere>

oppure
	./plive 
	
per vedere semplicemente i primi 10 numeri.