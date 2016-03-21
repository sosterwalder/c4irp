========
Go C4irp
========

Hallo Zusammen, ich nehme Chirp sehr ernst, ich glaube es so ziemlich das
grösste seit der Erfindung der Musikkassette. Ich habe mit Chirp Raft
implementiert und es war awesome!

Mit Chirp habe ich Actor-Based Programming wieder erfunden. Wusste nicht, dass
es das gibt, als ich es entworfen habe. Leider ist Chirp etwas zu langsam um
ernsthaft Actor-Based Programming zu machen. Erlang kann 3500000 Messages/s
durchlassen, während Chirp 3500/s schafft, das ist immer noch viel besser als
RabbitMQ (350/s). Aber ich möchte Faktor 10 - 1000 schneller sein.

Deshalb habe ich Concrete Clouds C Chirp gestartet. C4irp_. Leider geht so eine
C Implementation viel viel länger als die Python Variante. Wenn man mal wieder C
Programmieren muss, weiss man wie toll Python ist.

.. _C4irp: https://github.com/concretecloud/c4irp

Deshalb habe ich entschieden *CHF 20* pro Stunde für Unterstützung zu zahlen.

Wer kann helfen?
================

Grundsätzlich jeder den ich gut kenne. Bin da etwas heikel, da es um mein Geld
geht.

Wer kann was tun?
=================

* Wer C schreiben kann:

  - Implementieren
  - Dokumentieren
  - Testen
  - Bindings schreiben

* Wer C lesen kann:

  - Dokumentieren
  - Testen
  - Bindings schreiben

* Wer Python kann:

  - Dokumentieren
  - Testen
  - Bindings schreiben

Anfangs wäre ich froh einen der C schreiben kann zu kriegen. Was heisst C
schreiben zu können?

* Pointer verstehen
* Structs verstehen
* Enums verstehen
* Static verstehen

   - Wissen wann es sinnvoll ist

* Out Parameter (**) verstehen
* Opaque pointer verstehen (kann ich erklären wenn man obiges versteht)
* Minimale Kenntnisse der LibC (meine sind auch recht schlecht)

Wenn gewisse Dinge fehlen, erkläre ich es gerne.

Wie läuft das ab?
=================

Wir würden zusammensitzen und schauen woran man parallel arbeiten kann. Dann
wird ein Pull-Requests aufgemacht und ich werde immer mal wieder dreinreden,
wenn mir was am Pull-Reuqest nicht passt.

Wir werden nach N Stunden schauen wie weit der PR gekommen ist, da ich mein
Bankkonto nicht zu stark belasten kann.

Weiteres
========

* Wer für die CHF 20 arbeitet gibt mir das Copyright ab.
* Wer angefixt wird und ganz normal Open-Source mässig arbeitet will, kriegt
  geteiltes Copyright. Siehe_

.. _Siehe: https://concretecloud.github.io/quality-customers-licenses.html
