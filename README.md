SysOp
=====

[Gestion File Di Log]

Per quanto riguarda la registrazione di tutte le attività (errori e messaggi di informazione), in tutte e tre le utilities sviluppate facciamo uso del demone di sistema "syslog", il quale permette di salvare dei messaggi di log aventi una priorità stabilità al momento della chiamata, salvandoli nel percorso di default /var/log/syslog.
Questa scelta è dovuta al fatto che, con questo demone di sistema messo a disposizione da unix, i messaggi contengono la data e l'ora di quando sono stati generati, l'user id e il PID del processo che ha chimato la system call.
Inoltre è possibile differenziare messaggi di errore da messaggi di warning, informazione oppure di critical conditions.

Mkbkp [ PERANTONI ]
====


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