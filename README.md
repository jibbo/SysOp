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
Altrimenti, vengono letti tutti i files presenti nella prima cartella: ciascuno di esso viene confrontato con tutti i files presenti nella seconda cartella, 

	equal A B  mostra a video le differenze che ha la directory A rispetto alla directory B.
	equal A B  mostra a video le differenze che ha il (la) file (directory) B rispetto al (alla) file (directory) A.


Plive [ DE FRANCESCO ]
=====