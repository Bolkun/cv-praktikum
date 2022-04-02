
//	Clips Interface
// Clips with dynamic libraries

#include "CLIPSDLL.h"
#include <clips.h>

void main() {
	Environment *theEnv;
	theEnv = __CreateEnvironment();

	//__Load(theEnv, "Daten/ClipsProcessing.txt");

	__Build(theEnv,
		"(deffacts Regions"
		" (Ellipse Blau r1) (Ellipse Blau r2) (Dreieck Rot r3) (Sechseck Rot r4)"
		" (Ellipse Gelb r5)"
		" (Geo 10 20 3 r1) (Geo 20 20 3 r2) (Geo 15 15 4 r3) (Geo 15 10 5 r4)"
		" (Geo 15 15 12 r5))"
		);

	__Build(theEnv,
		"(deffunction Neben (?x1 ?y1 ?x2 ?y2) (and(< ?x1 ?x2)(= ?y1 ?y2)) )"
		);
	__Build(theEnv,
		"(deffunction Ueber (?x1 ?y1 ?x2 ?y2) (and(< ?y1 ?y2)(= ?x1 ?x2)) )"
		);

	__Build(theEnv,
		"(defrule Auge (Ellipse Blau ?o) => (assert(Auge ?o)) )"
		);
	__Build(theEnv,
		"(defrule Nase (Dreieck Rot ?o) => (assert(Nase ?o)) )"
		);
	__Build(theEnv,
		"(defrule Mund (Sechseck Rot ?o) => (assert(Mund ?o)) )"
		);
	__Build(theEnv,
		"(defrule Augenpaar"
		" (Auge ?o1) (Auge ?o2) (Geo ?x1 ?y1 ?d1 ?o1) (Geo ?x2 ?y2 ?d2 ?o2)"
		" (test(Neben ?x1 ?y1 ?x2 ?y2)) (test(= ?d1 ?d2))"
		" =>"
		"(bind ?o3(gensym)) (assert(Augenpaar ?o3))"
		" (assert(Geo (/(+ ?x1 ?x2)2) (/(+ ?y1 ?y2)2) (+ ?d1 ?d2) ?o3)) )"
		);
	__Build(theEnv,
		"(defrule Kopf  (Ellipse Gelb ?o1)  =>  (assert(Kopf ?o1)) )"
		);
	__Build(theEnv,
		"(defrule Gesicht"
		" (Mund ?o1) (Nase ?o2) (Augenpaar ?o3) (Kopf ?o4) "
		" (Geo ?x1 ?y1 ?d1 ?o1) (Geo ?x2 ?y2 ?d2 ?o2)"
		" (Geo ?x3 ?y3 ?d3 ?o3) (Geo ?x4 ?y4 ?d4 ?o4)"
		" (test(Ueber ?x1 ?y1 ?x2 ?y2))"
		" (test(Ueber ?x2 ?y2 ?x3 ?y3))"
		" (test(<(* ?d1 2)?d4)) (test(<(* ?d2 2)?d4)) (test(< ?d3 ?d4))"
		" =>"
		" (bind ?o5(gensym)) (assert(Gesicht ?o5 ?d4))"
		" (printout t \"Gesicht erkannt \") (printout t ?o5 \" \" ?d4 crlf) )"
		);
	//__Build(theEnv,
	//	"(retract 2 50)"
	//	);

	printf("\n%s\n", "Interpreter Begin");

	__Reset(theEnv);
	__Run(theEnv, -1);
	CLIPSValue resdat;
	int res;

	//res = __Eval(theEnv, "(Neben 1 2 3 4)", &resdat);
	res = __Eval(theEnv, "(facts)", &resdat);
	res = __Eval(theEnv, "(list-deffunctions)", &resdat);
	res = __Eval(theEnv, "(rules)", &resdat);
	res = __Eval(theEnv, "(ppfact 1)", &resdat);
	res = __Eval(theEnv, "(ppdeffunction Neben)", &resdat);
	res = __Eval(theEnv, "(ppdefrule Auge)", &resdat);
	res = __Eval(theEnv, "(fact-slot-names 1)", &resdat);

	Fact *f = __GetNextFact(theEnv, 0);
	f = __GetNextFact(theEnv, f);
	StringBuilder *sb;
	sb = __CreateStringBuilder(theEnv, 1000);
	__FactPPForm(f, sb, true);
	char resbuf[2000];
	sprintf_s(resbuf, "%s", sb->contents);
	printf("%s\n", resbuf);

	f = __GetNextFact(theEnv, f);
	__FactPPForm(f, sb, true);
	printf("%s\n", sb->contents);

	printf("\n%s\n", "Interpreter End");
	free(resbuf);
	__SBDispose(sb);
	__DestroyEnvironment(theEnv);
}
 