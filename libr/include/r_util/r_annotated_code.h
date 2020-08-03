
#ifndef R_ANNOTATEDCODE_H
#define R_ANNOTATEDCODE_H

// #include <r_core.h>
#include <r_types.h>
#include <r_vector.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum r_syntax_highlight_type_t {
	R_SYNTAX_HIGHLIGHT_TYPE_KEYWORD,
	R_SYNTAX_HIGHLIGHT_TYPE_COMMENT,
	R_SYNTAX_HIGHLIGHT_TYPE_DATATYPE,
	R_SYNTAX_HIGHLIGHT_TYPE_FUNCTION_NAME,
	R_SYNTAX_HIGHLIGHT_TYPE_FUNCTION_PARAMETER,
	R_SYNTAX_HIGHLIGHT_TYPE_LOCAL_VARIABLE,
	R_SYNTAX_HIGHLIGHT_TYPE_CONSTANT_VARIABLE,
	R_SYNTAX_HIGHLIGHT_TYPE_GLOBAL_VARIABLE,
} RSyntaxHighlightType;

/**
 * enum r_code_annotation_type_t - typedefed as RCodeAnnotationType and this gives types of annotation
 *
 * There are two kinds of RCodeAnnotation. One for offset, which of the type 
 * R_CODE_ANNOTATION_TYPE_OFFSET and other one is for syntax highlight, which is
 * of the type R_CODE_ANNOTATION_TYPE_SYNTAX_HIGHLIGHT.
 * R_CODE_ANNOTATION_TYPE_OFFSET is for representing annotations that gives an offset for 
 * a range while R_CODE_ANNOTATION_TYPE_SYNTAX_HIGHLIGHT is for representing the
 * kind of data the range represents. Here, range refers to the range of annotation.
 */

typedef enum r_code_annotation_type_t {
	R_CODE_ANNOTATION_TYPE_OFFSET,
	R_CODE_ANNOTATION_TYPE_SYNTAX_HIGHLIGHT,
	R_CODE_ANNOTATION_TYPE_FUNCTION_NAME,
	R_CODE_ANNOTATION_TYPE_GLOBAL_VARIABLE,
	R_CODE_ANNOTATION_TYPE_CONSTANT_VARIABLE,
	R_CODE_ANNOTATION_TYPE_LOCAL_VARIABLE,
	R_CODE_ANNOTATION_TYPE_FUNCTION_PARAMETER,
	// ...
} RCodeAnnotationType;

typedef struct r_code_annotation_t {
	size_t start;
	size_t end;
	RCodeAnnotationType type;
	union {
		struct {
			ut64 offset;
		} offset;

		struct {
			RSyntaxHighlightType type;
		} syntax_highlight;

		/** Information in annotations of type R_CODE_ANNOTATION_TYPE_FUNCTION_NAME,
		 * R_CODE_ANNOTATION_TYPE_GLOBAL_VARIABLE, and R_CODE_ANNOTATION_TYPE_CONSTANT_VARIABLE
		 * will be stored in the struct name reference in this union.
		 */
		struct {
			char *name;
			ut64 offset; // address
		} reference;

		/** Information in annotations of type R_CODE_ANNOTATION_TYPE_LOCAL_VARIABLE
		 * and R_CODE_ANNOTATION_TYPE_FUNCTION_PARAMETER will be stored in the 
		 * struct named variable in this union.
		 */
		struct {
			char *name;
		} variable;
	};
} RCodeAnnotation;

typedef struct r_annotated_code_t {
	char *code; // owned
	RVector /*<RCodeAnnotation>*/ annotations;
} RAnnotatedCode;
/**
 * This functions creates a new RAnnotatedCode structure.
 * RAnnotatedCode.code will be initialized as the character array passed.
 * Here, code must be a string that can be deallocated.
 * This will initialize RVector<RCodeAnnotation> annotations as well.
 * @brief Creates a new RAnnotatedCode structure and returns its pointer.
 * 
 * @param code Literal code for which the RAnnotatedCode structure will be created.
 * @return Pointer to the new RAnnotatedCode structure created.
 */
R_API RAnnotatedCode *r_annotated_code_new(char *code);
/**
 * This functions deallocates memory allocated for *code.
 * @brief Deallocates *code.
 * 
 * @param code Pointer to a RAnnotatedCode.
 */
R_API void r_annotated_code_free(RAnnotatedCode *code);
/**
 * This function recognizes the type of the specified annotation and
 * frees memory that is dynamically allocated for it.
 * @brief Deallocates dynamically allocated memory for the specified annotation.
 * 
 * @param e Pointer to the annotation.
 * @param user Always NULL for this function. Present here for this function to be of the type RVectorFree.
 */
R_API void r_annotation_free(void *e, void *user);
/**
 * This function recognizes the type of the specified annotation and returns true if its
 * type is any of the following three: R_CODE_ANNOTATION_TYPE_GLOBAL_VARIABLE,
 * R_CODE_ANNOTATION_TYPE_CONSTANT_VARIABLE, R_CODE_ANNOTATION_TYPE_FUNCTION_NAME
 * @brief Checks if the specified annotation is a reference.
 * 
 * @param annotation Pointer to an annotation.
 * @return Returns true if the specified annotation is a reference.
 */
R_API bool r_annotation_is_reference(RCodeAnnotation *annotation);
/**
 * This function recognizes the type of the specified annotation and returns true if its
 * type is any of the following two: R_CODE_ANNOTATION_TYPE_LOCAL_VARIABLE,
 * R_CODE_ANNOTATION_TYPE_FUNCTION_PARAMETER
 * @brief Checks if the specified annotation is a function variable.
 * 
 * @param annotation Pointer to an annotation.
 * @return Returns true if the specified annotation is a function variable.
 */
R_API bool r_annotation_is_variable(RCodeAnnotation *annotation);
/**
 * This functions inserts the annotation represented by the pointer 'annotation' to the vector
 * of annotations in the RAnnotatedCode represented by 'code'. To be more precise,
 * annotation will be added to code->annotations, which is a RVector<RCodeAnnotation> annotations.
 * @brief Inserts *annotation in *code.
 * 
 * @param code Pointer to a RAnnotatedCode.
 * @param annotation Pointer to an annotation.
 */
R_API void r_annotated_code_add_annotation(RAnnotatedCode *code, RCodeAnnotation *annotation);
/**
 * Creates an RPVector and inserts the pointers to all annotations in which 
 * annotation->start <= offset < annotation->end.
 * @brief Returns all annotations with range that contains the given offset.
 * 
 * @param code Pointer to a RAnnotatedCode.
 * @param offset Offset.
 * @return Pointer to the RPVector created.
 */
R_API RPVector *r_annotated_code_annotations_in(RAnnotatedCode *code, size_t offset);
/**
 * Creates an RPVector and inserts the pointers to all annotations whose 
 * range overlap with range [start, end-1] (both inclusive).
 * @brief Returns all annotations with range that overlap with the given range.
 * 
 * @param code Pointer to a RAnnotatedCode.
 * @param start Start of the range(inclusive).
 * @param end End of the range(exclusive).
 * @return Pointer to the RPVecrtor created.
 */
R_API RPVector *r_annotated_code_annotations_range(RAnnotatedCode *code, size_t start, size_t end);
/**
 * Creates an RVector and inserts the offsets for every seperate line of decompiled code in
 * code->code (code->code is a character array).
 * If a line of decompiled code doesn't have a unique offset, UT64_MAX is inserted as its offset.
 * @brief Returns the offset for every line of decompiled code in RAnnotatedCode *code.
 * 
 * @param code Pointer to a RAnnotatedCode.
 * @return Pointer to the RVector created.
 */
R_API RVector *r_annotated_code_line_offsets(RAnnotatedCode *code);

#ifdef __cplusplus
}
#endif

#endif //R_ANNOTATEDCODE_H
