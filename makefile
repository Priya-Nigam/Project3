#Priya Nigam CIS 3207 Project 3 SpellChecker

spell_checker: main.c Log_Queue.c Queue.c
	gcc main.c Log_Queue.c Queue.c -pthread -o spell_checker
