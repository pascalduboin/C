Fiche pratique : Intégration du minifyPHP dans CodeKit
(#MinifyPHP — en cours de dev)

— Prérequis

	1.	macOS avec CodeKit installé
	2.	Fichier source du minifier (minifier.c) prêt
	3.	 Accès au Terminal pour compiler et installer le binaire


— Compilation du minifier en binaire CLI

	1.	Se placer dans le dossier contenant minifier.c :
		cd /chemin/vers/ton/projet

	2.	Compiler avec optimisation :
		gcc -O3 -std=c11 -o phpminifier minifier.c
	
	3.	 Installer le binaire dans ton PATH :
		sudo mv phpminifier /usr/local/bin/
		chmod +x /usr/local/bin/phpminifier


— Configuration dans CodeKit


	1.	Ouvrir CodeKit et sélectionner ton projet.
	2.	Aller dans l’onglet Command Line Scripts (ou équivalent).
	3.	Ajouter un nouveau script :
	
		- Trigger : After Save (ou On File Save)
   	
   		- Files : Pattern `*.php`
  
   		- Command :
   		phpminifier "$INPUT_PATH" > "/chemin/vers/minified/(basename $INPUT_PATH)"

	4. Valider et sauvegarder la configuration



— Variables utiles de CodeKit

	Variable			Exemple
	---------------		---------------
	$INPUT_PATH		/Users/moi/Projets/site/index.php
	$INPUT_NAME		index.php
	$INPUT_DIR		/Users/moi/Projets/site
	[Input.Path]		équivalent de $INPUT_PATH
	[Input.Name]		équivalent de $INPUT_NAME



— Vérification & Debug

	1.	Test manuel :
		phpminifier src.php > build/src.php

	2.	Logs d’erreur :
		phpminifier "$INPUT_PATH" > "/dest/$(basename "$INPUT_PATH")" 2> /tmp/phpminifier.log
	
	3.	Permission :
		ls -l /usr/local/bin/phpminifier
		# doit afficher "-rwxr-xr-x"



— Astuces avancées

	1.	Contourner CodeKit :
		Utiliser fswatch ou entr pour watcher les .php et exécuter le minifier.
	
	2.	Batch : 
		Ajouter une cible make dans un Makefile pour compiler et minifier d’un coup.
	
	3.	Extensions :
		Adapter la commande pour d’autres types de fichiers (JS, CSS) si tu crées plusieurs binaires.
	
