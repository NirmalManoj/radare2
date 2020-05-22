#include <r_util.h>
#include <r_vector.h>
#include <r_core.h>
#include <r_cons.h>
#include <r_util/r_annotated_code.h>

#include "minunit.h"

static RCodeAnnotation makeRCodeAnnotation(int st, int en, RCodeAnnotationType typec, 
											ut64 offset, RSyntaxHighlightType types){
	RCodeAnnotation annotation = {};
	annotation.start = st;
	annotation.end = en;
	annotation.type = typec;
	if(annotation.type==R_CODE_ANNOTATION_TYPE_OFFSET){
		annotation.offset.offset = offset;
	}
	if(annotation.type==R_CODE_ANNOTATION_TYPE_SYNTAX_HIGHLIGHT){
		annotation.syntax_highlight.type=types;
	}
	return annotation;
}

static RVector getSomeAnnotationsForAdd(){
	RVector test_annotations;
	RCodeAnnotation annotation;
	r_vector_init (&test_annotations, sizeof(RCodeAnnotation), NULL, NULL);
	annotation = makeRCodeAnnotation(1, 2, R_CODE_ANNOTATION_TYPE_OFFSET, 123, R_SYNTAX_HIGHLIGHT_TYPE_KEYWORD);
	r_vector_push (&test_annotations, &annotation);
	annotation = makeRCodeAnnotation(1, 5, R_CODE_ANNOTATION_TYPE_SYNTAX_HIGHLIGHT, 123, R_SYNTAX_HIGHLIGHT_TYPE_KEYWORD);
	r_vector_push (&test_annotations, &annotation);
	return test_annotations;
}

static RVector getSomeAnnotationsForIn(){
	RVector test_annotations;
	RCodeAnnotation annotation;
	r_vector_init (&test_annotations, sizeof(RCodeAnnotation), NULL, NULL);
	annotation = makeRCodeAnnotation(1, 2, R_CODE_ANNOTATION_TYPE_OFFSET, 123, R_SYNTAX_HIGHLIGHT_TYPE_KEYWORD);
	r_vector_push (&test_annotations, &annotation);
	annotation = makeRCodeAnnotation(1, 7, R_CODE_ANNOTATION_TYPE_SYNTAX_HIGHLIGHT, 123, R_SYNTAX_HIGHLIGHT_TYPE_KEYWORD);
	r_vector_push (&test_annotations, &annotation);
	annotation = makeRCodeAnnotation(9, 11, R_CODE_ANNOTATION_TYPE_OFFSET, 123, R_SYNTAX_HIGHLIGHT_TYPE_KEYWORD);
	r_vector_push (&test_annotations, &annotation);

	// For offset = 11, indices expected = 3, 4, 5
	annotation = makeRCodeAnnotation(7, 13, R_CODE_ANNOTATION_TYPE_OFFSET, 123, R_SYNTAX_HIGHLIGHT_TYPE_KEYWORD);
	r_vector_push (&test_annotations, &annotation);
	annotation = makeRCodeAnnotation(11, 15, R_CODE_ANNOTATION_TYPE_OFFSET, 123, R_SYNTAX_HIGHLIGHT_TYPE_KEYWORD);
	r_vector_push (&test_annotations, &annotation);
	annotation = makeRCodeAnnotation(10, 16, R_CODE_ANNOTATION_TYPE_OFFSET, 123, R_SYNTAX_HIGHLIGHT_TYPE_KEYWORD);
	r_vector_push (&test_annotations, &annotation);
	annotation = makeRCodeAnnotation(17, 20, R_CODE_ANNOTATION_TYPE_OFFSET, 32, R_SYNTAX_HIGHLIGHT_TYPE_KEYWORD);
	r_vector_push (&test_annotations, &annotation);

	return test_annotations;
}

static RVector getSomeAnnotationsHelloWorld(){
	RVector test_annotations;
	RCodeAnnotation annotation;
	r_vector_init (&test_annotations, sizeof(RCodeAnnotation), NULL, NULL);
	//Code Annotations for a hello world program
	annotation = makeRCodeAnnotation(1, 5, R_CODE_ANNOTATION_TYPE_SYNTAX_HIGHLIGHT, 123, R_SYNTAX_HIGHLIGHT_TYPE_DATATYPE);
	r_vector_push (&test_annotations, &annotation); //1
	annotation = makeRCodeAnnotation(6, 10, R_CODE_ANNOTATION_TYPE_SYNTAX_HIGHLIGHT, 123, R_SYNTAX_HIGHLIGHT_TYPE_FUNCTION_NAME);
	r_vector_push (&test_annotations, &annotation); //2
	annotation = makeRCodeAnnotation(11, 15, R_CODE_ANNOTATION_TYPE_SYNTAX_HIGHLIGHT, 123, R_SYNTAX_HIGHLIGHT_TYPE_KEYWORD);
	r_vector_push (&test_annotations, &annotation); //3
	annotation = makeRCodeAnnotation(23, 35, R_CODE_ANNOTATION_TYPE_SYNTAX_HIGHLIGHT, 123, R_SYNTAX_HIGHLIGHT_TYPE_FUNCTION_NAME);
	r_vector_push (&test_annotations, &annotation); //4
	annotation = makeRCodeAnnotation(36, 51, R_CODE_ANNOTATION_TYPE_SYNTAX_HIGHLIGHT, 123, R_SYNTAX_HIGHLIGHT_TYPE_CONSTANT_VARIABLE);
	r_vector_push (&test_annotations, &annotation); //5
	annotation = makeRCodeAnnotation(23, 52, R_CODE_ANNOTATION_TYPE_OFFSET, 4440, R_SYNTAX_HIGHLIGHT_TYPE_KEYWORD);
	r_vector_push (&test_annotations, &annotation); //6
	annotation = makeRCodeAnnotation(58, 64, R_CODE_ANNOTATION_TYPE_OFFSET, 4447, R_SYNTAX_HIGHLIGHT_TYPE_KEYWORD);
	r_vector_push (&test_annotations, &annotation); //7
	annotation = makeRCodeAnnotation(58, 64, R_CODE_ANNOTATION_TYPE_SYNTAX_HIGHLIGHT, 123, R_SYNTAX_HIGHLIGHT_TYPE_KEYWORD);
	r_vector_push (&test_annotations, &annotation); //8
	annotation = makeRCodeAnnotation(58, 64, R_CODE_ANNOTATION_TYPE_OFFSET, 4447, R_SYNTAX_HIGHLIGHT_TYPE_KEYWORD);
	r_vector_push (&test_annotations, &annotation); //9	
	
	return test_annotations;
}


static RAnnotatedCode *getHelloWorld(){
	char *test_string = malloc(71 * sizeof(char));
	strcpy(test_string, "\nvoid main(void)\n{\n    sym.imp.puts(\"Hello, World!\");\n    return;\n}\n");
	RAnnotatedCode *code = r_annotated_code_new(test_string);

	RVector/*<RCodeAnnotation>*/ test_annotations;
	test_annotations = getSomeAnnotationsHelloWorld();
	RCodeAnnotation *annotation;
	r_vector_foreach (&test_annotations, annotation) {
		r_annotated_code_add_annotation(code, annotation);
	}
	return code;
}


static bool test_r_annotated_code_new(){

	//Testing RAnnoatedCode->code
	char *test_string = malloc(14 * sizeof(char));
	strcpy(test_string, "How are you?");
	RAnnotatedCode *code = r_annotated_code_new(test_string);
	mu_assert_streq(code->code, test_string, "Code in RAnnotatedCode is not set as expected");
	
	// Testing RAnnoatedCode->annotations
	mu_assert_eq(code->annotations.elem_size, sizeof(RCodeAnnotation), "Code Annotations are initialized is not properly");
	
	mu_end;
}

static bool test_r_annotated_code_free(){

	char *test_string = malloc(14 * sizeof(char));
	strcpy(test_string, "How are you?");
	RAnnotatedCode *code = r_annotated_code_new(test_string);

	RCodeAnnotation test_annotation1, test_annotation2;
	test_annotation1 = makeRCodeAnnotation(1, 2, R_CODE_ANNOTATION_TYPE_OFFSET, 123, R_SYNTAX_HIGHLIGHT_TYPE_KEYWORD);
	r_vector_push (&code->annotations, &test_annotation1);
	test_annotation2 = makeRCodeAnnotation(1, 5, R_CODE_ANNOTATION_TYPE_SYNTAX_HIGHLIGHT, 123, R_SYNTAX_HIGHLIGHT_TYPE_KEYWORD);
	r_vector_push (&code->annotations, &test_annotation2);
	
	// This test is only for run errors
	r_annotated_code_free(code);

	mu_end;
}	

static void test_equal(RCodeAnnotation *first, RCodeAnnotation *second){ // First - Got, Second - Expected
	mu_assert_eq(first->start, second->start, "start of annotations doesn't match");
	mu_assert_eq(first->end, second->end, "end of annotations doesn't match");
	mu_assert_eq(first->type, second->type, "type of annotation doesn't match");
	if(first->type == R_CODE_ANNOTATION_TYPE_OFFSET){
		mu_assert_eq(first->offset.offset, second->offset.offset, "offset of annotations doesn't match");
	}
	if(first->type == R_CODE_ANNOTATION_TYPE_SYNTAX_HIGHLIGHT){
		mu_assert_eq(first->syntax_highlight.type, second->syntax_highlight.type, "syntax highlight type of offset doesn't match");
	}
}

static bool test_r_annotated_code_add_annotation(){
	char *test_string = malloc(54 * sizeof(char));
	strcpy(test_string, "abcdefghijklmnopqrtstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	RAnnotatedCode *code = r_annotated_code_new(test_string);
	RVector/*<RCodeAnnotation>*/ test_annotations;
	test_annotations = getSomeAnnotationsForAdd();
	RCodeAnnotation *annotation;
	r_vector_foreach (&test_annotations, annotation) {
		r_annotated_code_add_annotation(code, annotation);
	}

	//Comparing
	test_equal(r_vector_index_ptr(&code->annotations, 0), r_vector_index_ptr(&test_annotations, 0));
	test_equal(r_vector_index_ptr(&code->annotations, 1), r_vector_index_ptr(&test_annotations, 1));
	
	mu_end;
}

static bool test_r_annotated_code_annotations_in(){
	char *test_string = malloc(54 * sizeof(char));
	strcpy(test_string, "abcdefghijklmnopqrtstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	RAnnotatedCode *code = r_annotated_code_new(test_string);
	RVector/*<RCodeAnnotation>*/ test_annotations;
	test_annotations = getSomeAnnotationsForIn();

	RCodeAnnotation *annotation;
	r_vector_foreach (&test_annotations, annotation) {
		r_annotated_code_add_annotation(code, annotation);
	}

	RPVector *out = r_annotated_code_annotations_in(code, 11);
	//Expecting indices = 3, 4, 5
	mu_assert_eq(out->v.len, 3, "Additional annotations found. Bad output.");

	test_equal(*r_pvector_index_ptr(out, 0), r_vector_index_ptr(&test_annotations, 3));
	test_equal(*r_pvector_index_ptr(out, 1), r_vector_index_ptr(&test_annotations, 4));
	test_equal(*r_pvector_index_ptr(out, 2), r_vector_index_ptr(&test_annotations, 5));

	mu_end;
}

static bool test_r_annotated_code_annotations_range(){
	char *test_string = malloc(54 * sizeof(char));
	strcpy(test_string, "abcdefghijklmnopqrtstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	RAnnotatedCode *code = r_annotated_code_new(test_string);
	RVector/*<RCodeAnnotation>*/ test_annotations;
	test_annotations = getSomeAnnotationsForIn();
	RCodeAnnotation *annotation;
	r_vector_foreach (&test_annotations, annotation) {
		r_annotated_code_add_annotation(code, annotation);
	}

	RPVector *out = r_annotated_code_annotations_range(code, 7, 16);
	// Expecting indices = 2, 3, 4, 5
	mu_assert_eq(out->v.len, 4, "Additional annotations found. Bad output.");

	test_equal(*r_pvector_index_ptr(out, 0), r_vector_index_ptr(&test_annotations, 2));
	test_equal(*r_pvector_index_ptr(out, 1), r_vector_index_ptr(&test_annotations, 3));
	test_equal(*r_pvector_index_ptr(out, 2), r_vector_index_ptr(&test_annotations, 4));
	test_equal(*r_pvector_index_ptr(out, 3), r_vector_index_ptr(&test_annotations, 5));
	mu_end;
}

static bool test_r_annotated_code_line_offsets(){

	RAnnotatedCode *code = getHelloWorld();
	RVector *offsets = r_annotated_code_line_offsets(code);
	mu_assert_eq(offsets->len, 6, "Number of offsets not expected");

	ut64 *off = r_vector_index_ptr(offsets, 0);
	mu_assert_eq_fmt(*off, UT64_MAX, "Unexpected offset", "%lu");
	off = r_vector_index_ptr(offsets, 1);
	mu_assert_eq_fmt(*off, UT64_MAX, "Unexpected offset", "%lu");
	off = r_vector_index_ptr(offsets, 2);	
	mu_assert_eq_fmt(*off, UT64_MAX, "Unexpected offset", "%lu");
	off = r_vector_index_ptr(offsets, 3);
	mu_assert_eq_fmt(*off, 4440, "Unexpected offset", "%lu");
	off = r_vector_index_ptr(offsets, 4);
	mu_assert_eq_fmt(*off, 4447, "Unexpected offset", "%lu");
	off = r_vector_index_ptr(offsets, 5);
	mu_assert_eq_fmt(*off, UT64_MAX, "Unexpected offset", "%lu");
	
	mu_end;
}

static bool test_r_annotated_code_print_json(){
	RAnnotatedCode *code = getHelloWorld();
	char *expected = "{\"code\":\"\\nvoid main(void)\\n{\\n    sym.imp.puts(\\\"Hello, World!\\\");\\n    return;\\n}\\n\",\"annotations\":[{\"start\":1,\"end\":5,\"type\":\"syntax_highlight\",\"syntax_highlight\":\"datatype\"},{\"start\":6,\"end\":10,\"type\":\"syntax_highlight\",\"syntax_highlight\":\"function_name\"},{\"start\":11,\"end\":15,\"type\":\"syntax_highlight\",\"syntax_highlight\":\"keyword\"},{\"start\":23,\"end\":35,\"type\":\"syntax_highlight\",\"syntax_highlight\":\"function_name\"},{\"start\":36,\"end\":51,\"type\":\"syntax_highlight\",\"syntax_highlight\":\"constant_variable\"},{\"start\":23,\"end\":52,\"type\":\"offset\",\"offset\":4440},{\"start\":58,\"end\":64,\"type\":\"offset\",\"offset\":4447},{\"start\":58,\"end\":64,\"type\":\"syntax_highlight\",\"syntax_highlight\":\"keyword\"},{\"start\":58,\"end\":64,\"type\":\"offset\",\"offset\":4447}]}\n";
	r_cons_new();
	r_cons_push();
	r_annotated_code_print_json(code);
	mu_assert_streq (r_cons_get_buffer(), expected, "pdgj OUTPUT DOES NOT MATCH");
	r_cons_pop();
	r_cons_flush();
	mu_end;
}

static bool test_r_annnotated_code_print(){
	RAnnotatedCode *code = getHelloWorld();
	//Checking without line offset
	char *expected_first = "\n"
							"void main(void)\n"
							"{\n"
							"    sym.imp.puts(\"Hello, World!\");\n"
							"    return;\n"
							"}\n";
	r_cons_new();
	r_cons_push();
	r_annotated_code_print(code, NULL);
	mu_assert_streq (r_cons_get_buffer(), expected_first, "pdg OUTPUT DOES NOT MATCH");
	r_cons_pop();

	//Checking with offset - pdgo
	RVector *offsets = r_annotated_code_line_offsets(code);
	char *expected_second = "                  |\n"
							"                  |void main(void)\n"
							"                  |{\n"
							"    0x00001158    |    sym.imp.puts(\"Hello, World!\");\n"
							"    0x0000115f    |    return;\n"
							"                  |}\n";
	r_annotated_code_print(code, offsets);
	mu_assert_streq (r_cons_get_buffer(), expected_second, "pdgo OUTPUT DOES NOT MATCH");
	r_cons_pop();
	mu_end;
}

static bool test_r_annotated_code_print_comment_cmds(){
	RAnnotatedCode *code = getHelloWorld();
	char *expected = "CCu base64:c3ltLmltcC5wdXRzKCJIZWxsbywgV29ybGQhIik= @ 0x1158\n"
                	 "CCu base64:cmV0dXJu @ 0x115f\n";
	r_cons_new();
	r_cons_push();
	r_annotated_code_print_comment_cmds(code);
	mu_assert_streq(r_cons_get_buffer(), expected, "pdg* OUTPUT DOES NOT MATCH");
	mu_end;
}

static int all_tests() {
    mu_run_test(test_r_annotated_code_new);
	mu_run_test(test_r_annotated_code_free);
	mu_run_test(test_r_annotated_code_add_annotation);
	mu_run_test(test_r_annotated_code_annotations_in);
	mu_run_test(test_r_annotated_code_annotations_range);
	mu_run_test(test_r_annotated_code_line_offsets);
	mu_run_test(test_r_annotated_code_print_json);
	mu_run_test(test_r_annnotated_code_print);
	mu_run_test(test_r_annotated_code_print_comment_cmds);
	return tests_passed != tests_run;
}

int main(int argc, char **argv) {
	return all_tests();
}