SysOp
=====


Equal [ ZEN ]
=====


Gestione log:

Viene utilizzata la logging facility chiamata syslog che genera dei messaggi di log aventi una priorità stabilità al momento della chiamata, salvandoli nel percorso di default /var/log.
Questa scelta è dovuta al fatto che con la facility messa a disposizione da unix i messaggi contengono la data e l'ora di quando sono stati generati, l'user id e l'id del processo che ha chimato la system call.