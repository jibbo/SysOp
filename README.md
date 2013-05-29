SysOp
=====


Equal [ ZEN ]
=====


Gestione log:


L'utility viene implementata in modo diverso dall'utility "diff" messa a disposizione da unix.
In questa versione l'esecuzione di:

	equal A B  mostra a video le differenze che ha il (la) file (directory) A rispetto al (alla) file (directory) B.
	equal A B  mostra a video le differenze che ha il (la) file (directory) B rispetto al (alla) file (directory) A.

Viene utilizzata la logging facility chiamata syslog che genera dei messaggi di log aventi una priorità stabilità al momento della chiamata, salvandoli nel percorso di default /var/log.
Questa scelta è dovuta al fatto che con la facility messa a disposizione da unix i messaggi contengono la data e l'ora di quando sono stati generati, l'user id e l'id del processo che ha chimato la system call.
Li butta in /var/log/syslog