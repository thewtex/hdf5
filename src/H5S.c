/****************************************************************************
* NCSA HDF								   *
* Software Development Group						   *
* National Center for Supercomputing Applications			   *
* University of Illinois at Urbana-Champaign				   *
* 605 E. Springfield, Champaign IL 61820				   *
*									   *
* For conditions of distribution and use, see the accompanying		   *
* hdf/COPYING file.							   *
*									   *
****************************************************************************/

#ifdef RCSID
static char		RcsId[] = "@(#)$Revision$";
#endif

/* $Id$ */

#include <H5private.h>		/* Generic Functions			  */
#include <H5Iprivate.h>		/* ID Functions		  */
#include <H5Eprivate.h>		/* Error handling		  */
#include <H5MMprivate.h>	/* Memory Management functions		  */
#include <H5Oprivate.h>		/* object headers		  */
#include <H5Sprivate.h>		/* Data-space functions			  */

/* Interface initialization */
#define PABLO_MASK	H5S_mask
#define INTERFACE_INIT	H5S_init_interface
static intn		interface_initialize_g = FALSE;
static herr_t		H5S_init_interface(void);
static void		H5S_term_interface(void);


/*--------------------------------------------------------------------------
NAME
   H5S_init_interface -- Initialize interface-specific information
USAGE
    herr_t H5S_init_interface()
   
RETURNS
   SUCCEED/FAIL
DESCRIPTION
    Initializes any interface-specific data or routines.

--------------------------------------------------------------------------*/
static herr_t
H5S_init_interface(void)
{
    herr_t		    ret_value = SUCCEED;
    FUNC_ENTER(H5S_init_interface, FAIL);

    /* Initialize the atom group for the file IDs */
    if ((ret_value = H5I_init_group(H5_DATASPACE, H5I_DATASPACEID_HASHSIZE,
            H5S_RESERVED_ATOMS, (herr_t (*)(void *)) H5S_close)) != FAIL) {
        ret_value = H5_add_exit(&H5S_term_interface);
    }
    FUNC_LEAVE(ret_value);
}


/*--------------------------------------------------------------------------
 NAME
    H5S_term_interface
 PURPOSE
    Terminate various H5S objects
 USAGE
    void H5S_term_interface()
 RETURNS
    SUCCEED/FAIL
 DESCRIPTION
    Release the atom group and any other resources allocated.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
     Can't report errors...
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
static void
H5S_term_interface(void)
{
    H5I_destroy_group(H5_DATASPACE);
}

/*--------------------------------------------------------------------------
 NAME
    H5S_create
 PURPOSE
    Create empty, typed dataspace
 USAGE
   H5S_t *H5S_create(type)
    H5S_type_t  type;           IN: Dataspace type to create
 RETURNS
    Pointer to dataspace on success, NULL on failure
 DESCRIPTION
    Creates a new dataspace of a given type.  The extent & selection are
    undefined
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
H5S_t *
H5S_create(H5S_class_t type)
{
    H5S_t *ret_value = NULL;

    FUNC_ENTER(H5S_create, NULL);

    /* Create a new data space */
    if((ret_value = H5MM_calloc(sizeof(H5S_t)))!=NULL)
    {
        ret_value->extent.type = type;
        ret_value->select.type = H5S_SEL_ALL;  /* Entire extent selected by default */
    }

#ifdef LATER
done:
#endif
    FUNC_LEAVE(ret_value);
} /* end H5S_create() */

/*--------------------------------------------------------------------------
 NAME
    H5Screate
 PURPOSE
    Create empty, typed dataspace
 USAGE
   hid_t  H5Screate(type)
    H5S_type_t  type;           IN: Dataspace type to create
 RETURNS
    Valid dataspace ID on success, negative on failure
 DESCRIPTION
    Creates a new dataspace of a given type.  The extent & selection are
    undefined
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
hid_t
H5Screate (H5S_class_t type)
{
    H5S_t  *new_ds=NULL;
    hid_t	ret_value = FAIL;

    FUNC_ENTER(H5Screate, FAIL);
    H5TRACE1("i","Sc",type);

    /* Check args */
    if(type<=H5S_NO_CLASS || type> H5S_SIMPLE)  /* don't allow complex dataspace yet */
        HRETURN_ERROR (H5E_ARGS, H5E_BADVALUE, FAIL,
                   "invalid dataspace type");

    if (NULL==(new_ds=H5S_create(type))) {
        HRETURN_ERROR (H5E_DATASPACE, H5E_CANTCREATE, FAIL, "unable to create dataspace");
    }

    /* Atomize */
    if ((ret_value=H5I_register (H5_DATASPACE, new_ds))<0) {
        HGOTO_ERROR (H5E_ATOM, H5E_CANTREGISTER, FAIL, "unable to register data space atom");
    }

done:
    if (ret_value < 0) {
    }
    FUNC_LEAVE(ret_value);
} /* end H5Screate() */

/*-------------------------------------------------------------------------
 * Function:	H5S_close
 *
 * Purpose:	Releases all memory associated with a data space.
 *
 * Return:	Success:	SUCCEED
 *
 *		Failure:	FAIL
 *
 * Programmer:	Robb Matzke
 *		Tuesday, December  9, 1997
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5S_close(H5S_t *ds)
{
    FUNC_ENTER(H5S_close, FAIL);

    assert(ds);

    /* Release selection (this should come before the extent release) */
    H5S_select_release(ds);

    /* release extent */
    switch (ds->extent.type) {
        case H5S_NO_CLASS:
            /*nothing needed */
            break;

        case H5S_SCALAR:
            /*nothing needed */
            break;

        case H5S_SIMPLE:
            H5S_release_simple(&(ds->extent.u.simple));
            break;

        case H5S_COMPLEX:
            /* nothing yet */
            break;

        default:
            assert("unknown dataspace (extent) type" && 0);
            break;
    }

    /* Release the main structure */
    H5MM_xfree(ds);

    FUNC_LEAVE(SUCCEED);
}

/*-------------------------------------------------------------------------
 * Function:	H5Sclose
 *
 * Purpose:	Release access to a data space object.
 *
 * Return:	Success:	SUCCEED
 *
 *		Failure:	FAIL
 *
 * Errors:
 *
 * Programmer:	Robb Matzke
 *		Tuesday, December  9, 1997
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5Sclose (hid_t space_id)
{
    FUNC_ENTER(H5Sclose, FAIL);
    H5TRACE1("e","i",space_id);

    /* Check args */
    if (H5_DATASPACE != H5I_group(space_id) ||
        NULL == H5I_object(space_id)) {
        HRETURN_ERROR(H5E_ARGS, H5E_BADTYPE, FAIL, "not a data space");
    }
    /* When the reference count reaches zero the resources are freed */
    if (H5I_dec_ref(space_id) < 0) {
        HRETURN_ERROR(H5E_ATOM, H5E_BADATOM, FAIL, "problem freeing id");
    }
    FUNC_LEAVE(SUCCEED);
}

/*-------------------------------------------------------------------------
 * Function:	H5S_release_simple
 *
 * Purpose:	Releases all memory associated with a simple data space.
 *          (but doesn't free the simple space itself)
 *
 * Return:	Success:	SUCCEED
 *
 *		Failure:	FAIL
 *
 * Programmer:	Quincey Koziol
 *		Friday, April  17, 1998
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5S_release_simple(H5S_simple_t *simple)
{
    FUNC_ENTER(H5S_release_simple, FAIL);

    assert(simple);

    if(simple->size)
        H5MM_xfree(simple->size);
    if(simple->max)
        H5MM_xfree(simple->max);

    FUNC_LEAVE(SUCCEED);
}

/*-------------------------------------------------------------------------
 * Function:	H5Scopy
 *
 * Purpose:	Copies a dataspace.
 *
 * Return:	Success:	ID of the new dataspace
 *
 *		Failure:	FAIL
 *
 * Programmer:	Robb Matzke
 *		Friday, January 30, 1998
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
hid_t
H5Scopy (hid_t space_id)
{
    H5S_t	*src = NULL;
    H5S_t	*dst = NULL;
    hid_t	ret_value = FAIL;
    
    FUNC_ENTER (H5Scopy, FAIL);
    H5TRACE1("i","i",space_id);

    /* Check args */
    if (H5_DATASPACE!=H5I_group (space_id) || NULL==(src=H5I_object (space_id))) {
        HRETURN_ERROR (H5E_ARGS, H5E_BADTYPE, FAIL, "not a data space");
    }

    /* Copy */
    if (NULL==(dst=H5S_copy (src))) {
        HRETURN_ERROR (H5E_DATASPACE, H5E_CANTINIT, FAIL, "unable to copy data space");
    }

    /* Atomize */
    if ((ret_value=H5I_register (H5_DATASPACE, dst))<0) {
        HRETURN_ERROR (H5E_ATOM, H5E_CANTREGISTER, FAIL, "unable to register data space atom");
    }

    FUNC_LEAVE (ret_value);
}


/*-------------------------------------------------------------------------
 * Function:	H5S_extent_copy
 *
 * Purpose:	Copies a dataspace extent
 *
 * Return:	SUCCEED/FAIL
 *
 * Programmer:	Quincey Koziol
 *		Wednesday, June  3, 1998
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5S_extent_copy(H5S_extent_t *dst, const H5S_extent_t *src)
{
    int			    i;

    FUNC_ENTER(H5S_extent_copy, FAIL);

    /* Copy the regular fields */
    *dst=*src;

    switch (src->type) {
        case H5S_SCALAR:
            /*nothing needed */
            break;

        case H5S_SIMPLE:
            if (src->u.simple.size) {
                dst->u.simple.size = H5MM_malloc(src->u.simple.rank *
                                  sizeof(src->u.simple.size[0]));
                for (i = 0; i < src->u.simple.rank; i++) {
                    dst->u.simple.size[i] = src->u.simple.size[i];
                }
            }
            if (src->u.simple.max) {
                dst->u.simple.max = H5MM_malloc(src->u.simple.rank *
                                 sizeof(src->u.simple.max[0]));
                for (i = 0; i < src->u.simple.rank; i++) {
                    dst->u.simple.max[i] = src->u.simple.max[i];
                }
            }
            break;

        case H5S_COMPLEX:
            /*void */
            break;

        default:
            assert("unknown data space type" && 0);
            break;
    }

    FUNC_LEAVE(SUCCEED);
}

/*-------------------------------------------------------------------------
 * Function:	H5S_copy
 *
 * Purpose:	Copies a data space, by copying the extent and selection through
 *          H5S_extent_copy and H5S_select_copy
 *
 * Return:	Success:	A pointer to a new copy of SRC
 *
 *		Failure:	NULL
 *
 * Programmer:	Robb Matzke
 *		Thursday, December  4, 1997
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
H5S_t *
H5S_copy(const H5S_t *src)
{
    H5S_t		   *dst = NULL;

    FUNC_ENTER(H5S_copy, NULL);

    if (NULL==(dst = H5MM_malloc(sizeof(H5S_t)))) {
	HRETURN_ERROR (H5E_RESOURCE, H5E_NOSPACE, NULL,
		       "memory allocation failed");
    }
    *dst = *src;

    /* Copy the source dataspace's extent */
    if (H5S_extent_copy(&(dst->extent),&(src->extent))<0)
        HRETURN_ERROR(H5E_DATASPACE, H5E_CANTCOPY, NULL, "can't copy extent");

    /* Copy the source dataspace's selection */
    if (H5S_select_copy(dst,src)<0)
        HRETURN_ERROR(H5E_DATASPACE, H5E_CANTCOPY, NULL, "can't copy select");

    FUNC_LEAVE(dst);
}


/*-------------------------------------------------------------------------
 * Function:	H5S_extent_npoints
 *
 * Purpose:	Determines how many data points a dataset extent has.
 *
 * Return:	Success:	Number of data points in the dataset extent.
 *
 *		Failure:	0
 *
 * Programmer:	Robb Matzke
 *		Tuesday, December  9, 1997
 *
 * Modifications:
 *  Changed Name - QAK 7/7/98
 *
 *-------------------------------------------------------------------------
 */
hsize_t
H5S_extent_npoints(const H5S_t *ds)
{
    hsize_t		    ret_value = 0;
    intn		    i;

    FUNC_ENTER(H5S_extent_npoints, 0);

    /* check args */
    assert(ds);

    switch (ds->extent.type) {
        case H5S_SCALAR:
            ret_value = 1;
            break;

        case H5S_SIMPLE:
            for (ret_value=1, i=0; i<ds->extent.u.simple.rank; i++) {
                ret_value *= ds->extent.u.simple.size[i];
            }
            break;

        case H5S_COMPLEX:
            HRETURN_ERROR(H5E_DATASPACE, H5E_UNSUPPORTED, 0,
                  "complex data spaces are not supported yet");

        default:
            assert("unknown data space class" && 0);
            HRETURN_ERROR(H5E_DATASPACE, H5E_UNSUPPORTED, 0,
                  "internal error (unknown data space class)");
    }

    FUNC_LEAVE(ret_value);
}


/*-------------------------------------------------------------------------
 * Function:	H5Sextent_npoints
 *
 * Purpose:	Determines how many data points a dataset extent has.
 *
 * Return:	Success:	Number of data points in the dataset.
 *
 *		Failure:	0
 *
 * Programmer:	Robb Matzke
 *		Tuesday, December  9, 1997
 *
 * Modifications:
 *  Changed Name - QAK 7/7/98
 *
 *-------------------------------------------------------------------------
 */
hsize_t
H5Sextent_npoints (hid_t space_id)
{
    H5S_t		   *ds = NULL;
    hsize_t		    ret_value = 0;

    FUNC_ENTER(H5Sextent_npoints, 0);
    H5TRACE1("h","i",space_id);

    /* Check args */
    if (H5_DATASPACE != H5I_group(space_id) || NULL == (ds = H5I_object(space_id))) {
        HRETURN_ERROR(H5E_ARGS, H5E_BADTYPE, 0, "not a data space");
    }
    ret_value = H5S_extent_npoints(ds);

    FUNC_LEAVE(ret_value);
}

/*-------------------------------------------------------------------------
 * Function:	H5S_get_npoints_max
 *
 * Purpose:	Determines the maximum number of data points a data space may
 *		have.  If the `max' array is null then the maximum number of
 *		data points is the same as the current number of data points
 *		without regard to the hyperslab.  If any element of the `max'
 *		array is zero then the maximum possible size is returned.
 *
 * Return:	Success:	Maximum number of data points the data space
 *				may have.
 *
 *		Failure:	0
 *
 * Programmer:	Robb Matzke
 *		Tuesday, December  9, 1997
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
hsize_t
H5S_get_npoints_max(const H5S_t *ds)
{
    hsize_t		    ret_value = 0;
    intn		    i;

    FUNC_ENTER(H5S_get_npoints_max, 0);

    /* check args */
    assert(ds);

    switch (ds->extent.type) {
        case H5S_SCALAR:
            ret_value = 1;
            break;

        case H5S_SIMPLE:
            if (ds->extent.u.simple.max) {
                for (ret_value=1, i=0; i<ds->extent.u.simple.rank; i++) {
                    if (H5S_UNLIMITED==ds->extent.u.simple.max[i]) {
                        ret_value = MAX_HSIZET;
                        break;
                    } else {
                        ret_value *= ds->extent.u.simple.max[i];
                    }
                }
            } else {
                for (ret_value=1, i=0; i<ds->extent.u.simple.rank; i++) {
                    ret_value *= ds->extent.u.simple.size[i];
                }
            }
            break;

        case H5S_COMPLEX:
            HRETURN_ERROR(H5E_DATASPACE, H5E_UNSUPPORTED, 0,
                  "complex data spaces are not supported yet");

        default:
            assert("unknown data space class" && 0);
            HRETURN_ERROR(H5E_DATASPACE, H5E_UNSUPPORTED, 0,
                  "internal error (unknown data space class)");
    }

    FUNC_LEAVE(ret_value);
}


/*-------------------------------------------------------------------------
 * Function:	H5Sget_ndims
 *
 * Purpose:	Determines the dimensionality of a data space.
 *
 * Return:	Success:	The number of dimensions in a data space.
 *
 *		Failure:	FAIL
 *
 * Programmer:	Robb Matzke
 *		Thursday, December 11, 1997
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
int
H5Sget_ndims (hid_t space_id)
{
    H5S_t		   *ds = NULL;
    intn		   ret_value = 0;

    FUNC_ENTER(H5Sget_ndims, FAIL);
    H5TRACE1("Is","i",space_id);

    /* Check args */
    if (H5_DATASPACE != H5I_group(space_id) || NULL == (ds = H5I_object(space_id))) {
        HRETURN_ERROR(H5E_ARGS, H5E_BADTYPE, FAIL, "not a data space");
    }
    ret_value = H5S_get_ndims(ds);

    FUNC_LEAVE(ret_value);
}

/*-------------------------------------------------------------------------
 * Function:	H5S_get_ndims
 *
 * Purpose:	Returns the number of dimensions in a data space.
 *
 * Return:	Success:	Non-negative number of dimensions.  Zero
 *				implies a scalar.
 *
 *		Failure:	FAIL
 *
 * Programmer:	Robb Matzke
 *		Thursday, December 11, 1997
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
intn
H5S_get_ndims(const H5S_t *ds)
{
    intn		    ret_value = FAIL;

    FUNC_ENTER(H5S_get_ndims, FAIL);

    /* check args */
    assert(ds);

    switch (ds->extent.type) {
        case H5S_SCALAR:
            ret_value = 0;
            break;

        case H5S_SIMPLE:
            ret_value = ds->extent.u.simple.rank;
            break;

        case H5S_COMPLEX:
            HRETURN_ERROR(H5E_DATASPACE, H5E_UNSUPPORTED, FAIL,
                  "complex data spaces are not supported yet");

        default:
            assert("unknown data space class" && 0);
            HRETURN_ERROR(H5E_DATASPACE, H5E_UNSUPPORTED, FAIL,
                  "internal error (unknown data space class)");
    }

    FUNC_LEAVE(ret_value);
}

/*-------------------------------------------------------------------------
 * Function:	H5Sget_dims
 *
 * Purpose:	Returns the size and maximum sizes in each dimension of
 *		a data space DS through	the DIMS and MAXDIMS arguments.
 *
 * Return:	Success:	Number of dimensions, the same value as
 *				returned by H5Sget_ndims().
 *
 *		Failure:	FAIL
 *
 * Programmer:	Robb Matzke
 *		Thursday, December 11, 1997
 *
 * Modifications:
 *		June 18, 1998	Albert Cheng
 *		Added maxdims argument.  Removed dims argument check
 *		since it can still return ndims even if both dims and
 *		maxdims are NULLs.
 *
 *-------------------------------------------------------------------------
 */
int
H5Sget_dims (hid_t space_id, hsize_t dims[]/*out*/, hsize_t maxdims[]/*out*/)
{
    H5S_t		   *ds = NULL;
    intn		   ret_value = 0;

    FUNC_ENTER(H5Sget_dims, FAIL);
    H5TRACE3("Is","ixx",space_id,dims,maxdims);

    /* Check args */
    if (H5_DATASPACE != H5I_group(space_id) || NULL == (ds = H5I_object(space_id))) {
        HRETURN_ERROR(H5E_ARGS, H5E_BADTYPE, FAIL, "not a data space");
    }
    ret_value = H5S_get_dims(ds, dims, maxdims);

    FUNC_LEAVE(ret_value);
}

/*-------------------------------------------------------------------------
 * Function:	H5S_get_dims
 *
 * Purpose:	Returns the size in each dimension of a data space.  This
 *		function may not be meaningful for all types of data spaces.
 *
 * Return:	Success:	Number of dimensions.  Zero implies scalar.
 *
 *		Failure:	FAIL
 *
 * Programmer:	Robb Matzke
 *		Thursday, December 11, 1997
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
intn
H5S_get_dims(const H5S_t *ds, hsize_t dims[], hsize_t max_dims[])
{
    intn	ret_value = FAIL;
    intn	i;

    FUNC_ENTER(H5S_get_dims, FAIL);

    /* check args */
    assert(ds);

    switch (ds->extent.type) {
        case H5S_SCALAR:
            ret_value = 0;
            break;

        case H5S_SIMPLE:
            ret_value = ds->extent.u.simple.rank;
            for (i=0; i<ret_value; i++) {
                if (dims)
                    dims[i] = ds->extent.u.simple.size[i];
                if (max_dims) {
                    if (ds->extent.u.simple.max) {
                        max_dims[i] = ds->extent.u.simple.max[i];
                    } else {
                        max_dims[i] = ds->extent.u.simple.size[i];
                    }
                }
            }
            break;

        case H5S_COMPLEX:
            HRETURN_ERROR(H5E_DATASPACE, H5E_UNSUPPORTED, FAIL,
                  "complex data spaces are not supported yet");

        default:
            assert("unknown data space class" && 0);
            HRETURN_ERROR(H5E_DATASPACE, H5E_UNSUPPORTED, FAIL,
                  "internal error (unknown data space class)");
    }

    FUNC_LEAVE(ret_value);
}

/*-------------------------------------------------------------------------
 * Function:	H5S_modify
 *
 * Purpose:	Updates a data space by writing a message to an object
 *		header.
 *
 * Return:	Success:	SUCCEED
 *
 *		Failure:	FAIL
 *
 * Programmer:	Robb Matzke
 *		Tuesday, December  9, 1997
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5S_modify(H5G_entry_t *ent, const H5S_t *ds)
{
    FUNC_ENTER(H5S_modify, FAIL);

    assert(ent);
    assert(ds);

    switch (ds->extent.type) {
        case H5S_SCALAR:
        case H5S_SIMPLE:
            if (H5O_modify(ent, H5O_SDSPACE, 0, 0, &(ds->extent.u.simple)) < 0) {
                HRETURN_ERROR(H5E_DATASPACE, H5E_CANTINIT, FAIL,
                      "can't update simple data space message");
            }
            break;

        case H5S_COMPLEX:
            HRETURN_ERROR(H5E_DATASPACE, H5E_UNSUPPORTED, FAIL,
                  "complex data spaces are not implemented yet");

        default:
            assert("unknown data space class" && 0);
            break;
    }

    FUNC_LEAVE(SUCCEED);
}

/*-------------------------------------------------------------------------
 * Function:	H5S_read
 *
 * Purpose:	Reads the data space from an object header.
 *
 * Return:	Success:	Pointer to a new data space.
 *
 *		Failure:	NULL
 *
 * Programmer:	Robb Matzke
 *		Tuesday, December  9, 1997
 *
 * Modifications:
 *	Robb Matzke, 9 Jun 1998
 *	Removed the unused file argument since the file is now part of the
 *	ENT argument.
 *-------------------------------------------------------------------------
 */
H5S_t *
H5S_read(H5G_entry_t *ent)
{
    H5S_t		   *ds = NULL;

    FUNC_ENTER(H5S_read, NULL);

    /* check args */
    assert(ent);

    if (NULL==(ds = H5MM_calloc(sizeof(H5S_t)))) {
        HRETURN_ERROR (H5E_RESOURCE, H5E_NOSPACE, NULL,
		       "memory allocation failed");
    }
    
    if (H5O_read(ent, H5O_SDSPACE, 0, &(ds->extent.u.simple))) {
        ds->extent.type = H5S_SIMPLE;
    } else {
        ds->extent.type = H5S_SCALAR;
    }

    /* Default to entire dataspace being selected */
    ds->select.type=H5S_SEL_ALL;

    FUNC_LEAVE(ds);
}

/*-------------------------------------------------------------------------
 * Function:	H5S_cmp
 *
 * Purpose:	Compares two data space extents.
 *
 * Return:	Success:	0 if DS1 and DS2 are the same.
 *				<0 if DS1 is less than DS2.
 *				>0 if DS1 is greater than DS2.
 *
 *		Failure:	0, never fails
 *
 * Programmer:	Robb Matzke
 *		Wednesday, December 10, 1997
 *
 * Modifications:
 *      6/9/98 Changed to only compare extents - QAK
 *
 *-------------------------------------------------------------------------
 */
intn
H5S_cmp(const H5S_t *ds1, const H5S_t *ds2)
{
    intn		    i;

    FUNC_ENTER(H5S_cmp, 0);

    /* check args */
    assert(ds1);
    assert(ds2);

    /* compare */
    if (ds1->extent.type < ds2->extent.type)
        HRETURN(-1);
    if (ds1->extent.type > ds2->extent.type)
        HRETURN(1);

    switch (ds1->extent.type) {
        case H5S_SIMPLE:
            if (ds1->extent.u.simple.rank < ds2->extent.u.simple.rank)
                HRETURN(-1);
            if (ds1->extent.u.simple.rank > ds2->extent.u.simple.rank)
                HRETURN(1);

            for (i = 0; i < ds1->extent.u.simple.rank; i++) {
                if (ds1->extent.u.simple.size[i] < ds2->extent.u.simple.size[i])
                    HRETURN(-1);
                if (ds1->extent.u.simple.size[i] > ds2->extent.u.simple.size[i])
                    HRETURN(1);
            }

            /* don't compare max dimensions */

#ifdef LATER
            for (i = 0; i < ds1->extent.u.simple.rank; i++) {
                if ((ds1->extent.u.simple.perm ? ds1->extent.u.simple.perm[i] : i) <
                        (ds2->extent.u.simple.perm ? ds2->extent.u.simple.perm[i] : i))
                    HRETURN(-1);
                if ((ds1->extent.u.simple.perm ? ds2->extent.u.simple.perm[i] : i) >
                        (ds2->extent.u.simple.perm ? ds2->extent.u.simple.perm[i] : i))
                    HRETURN(1);
            }
#endif

            break;

        default:
            assert("not implemented yet" && 0);
    }

    FUNC_LEAVE(0);
}


/*--------------------------------------------------------------------------
 NAME
    H5S_is_simple
 PURPOSE
    Check if a dataspace is simple (internal)
 USAGE
    hbool_t H5S_is_simple(sdim)
	H5S_t *sdim;		IN: Pointer to dataspace object to query
 RETURNS
    TRUE/FALSE/FAIL
 DESCRIPTION
	This function determines the if a dataspace is "simple". ie. if it
    has orthogonal, evenly spaced dimensions.
--------------------------------------------------------------------------*/
hbool_t
H5S_is_simple(const H5S_t *sdim)
{
    hbool_t		    ret_value = FAIL;

    FUNC_ENTER(H5S_is_simple, FAIL);

    /* Check args and all the boring stuff. */
    assert(sdim);
    ret_value = sdim->extent.type == H5S_SIMPLE ? TRUE : FALSE;

    FUNC_LEAVE(ret_value);
}


/*--------------------------------------------------------------------------
 NAME
    H5Sis_simple
 PURPOSE
    Check if a dataspace is simple
 USAGE
    hbool_t H5Sis_simple(sid)
	hid_t sid;	      IN: ID of dataspace object to query
 RETURNS
    TRUE/FALSE/FAIL
 DESCRIPTION
	This function determines the if a dataspace is "simple". ie. if it
    has orthogonal, evenly spaced dimensions.
--------------------------------------------------------------------------*/
hbool_t
H5Sis_simple (hid_t sid)
{
    H5S_t		   *space = NULL;	/* dataspace to modify */
    hbool_t		    ret_value = FAIL;

    FUNC_ENTER(H5Sis_simple, FAIL);
    H5TRACE1("b","i",sid);

    /* Check args and all the boring stuff. */
    if ((space = H5I_object(sid)) == NULL)
	HGOTO_ERROR(H5E_ATOM, H5E_BADATOM, FAIL, "not a data space");

    ret_value = H5S_is_simple(space);

  done:
    if (ret_value == FAIL) {   /* Error condition cleanup */

    }				/* end if */
    /* Normal function cleanup */
    FUNC_LEAVE(ret_value);
}


/*--------------------------------------------------------------------------
 NAME
    H5Sset_extent_simple
 PURPOSE
    Sets the size of a simple dataspace
 USAGE
    herr_t H5Sset_extent_simple(sid, rank, dims, max)
        hid_t sid;	      IN: Dataspace object to query
        intn rank;	      IN: # of dimensions for the dataspace
        const size_t *dims;   IN: Size of each dimension for the dataspace
        const size_t *max;    IN: Maximum size of each dimension for the dataspace
 RETURNS
    SUCCEED/FAIL
 DESCRIPTION
	This function sets the number and size of each dimension in the
    dataspace.	Setting RANK to a value of zero converts the dataspace to a
    scalar dataspace.  Dimensions are specified from slowest to fastest changing
    in the DIMS array (i.e. 'C' order).  Setting the size of a dimension in the
    MAX array to zero indicates that the dimension is of unlimited size and
    should be allowed to expand.  If MAX is NULL, the dimensions in the DIMS
    array are used as the maximum dimensions.  Currently, only the first
    dimension in the array (the slowest) may be unlimited in size.
--------------------------------------------------------------------------*/
herr_t
H5Sset_extent_simple (hid_t sid, int rank, const hsize_t *dims,
		      const hsize_t *max)
{
    H5S_t		   *space = NULL;	/* dataspace to modify */
    intn		    u;	/* local counting variable */

    FUNC_ENTER(H5Sset_extent_simple, FAIL);
    H5TRACE4("e","iIs*h*h",sid,rank,dims,max);

    /* Check args */
    if ((space = H5I_object(sid)) == NULL) {
        HRETURN_ERROR(H5E_ATOM, H5E_BADATOM, FAIL, "not a data space");
    }
    if (rank > 0 && dims == NULL) {
        HRETURN_ERROR(H5E_ARGS, H5E_BADVALUE, FAIL, "no dimensions specified");
    }
    if (rank<0) {
        HRETURN_ERROR (H5E_ARGS, H5E_BADVALUE, FAIL, "invalid rank");
    }
    if (dims) {
        for (u=0; u<rank; u++) {
            if (((max!=NULL && max[u]!=H5S_UNLIMITED) || max==NULL) &&
		dims[u]==0) {
                HRETURN_ERROR (H5E_ARGS, H5E_BADVALUE, FAIL,
			       "invalid dimension size");
            }
        }
    }
    if (max!=NULL) {
        if(dims==NULL) {
            HRETURN_ERROR (H5E_ARGS, H5E_BADVALUE, FAIL,
			   "maximum dimension specified, but no current "
			   "dimensions specified");
	}
        for (u=0; u<rank; u++) {
            if (max[u]!=H5S_UNLIMITED && max[u]<dims[u]) {
                HRETURN_ERROR (H5E_ARGS, H5E_BADVALUE, FAIL,
			       "invalid maximum dimension size");
            }
        }
    }

    /* Do it */
    if (H5S_set_extent_simple(space, rank, dims, max)<0) {
	HRETURN_ERROR(H5E_DATASPACE, H5E_CANTINIT, FAIL,
		      "unable to set simple extent");
    }

    FUNC_LEAVE(SUCCEED);
}


/*-------------------------------------------------------------------------
 * Function:	H5S_set_extent_simple
 *
 * Purpose:	This is where the real work happens for
 *		H5Sset_extent_simple().
 *
 * Return:	Success:	SUCCEED
 *
 *		Failure:	FAIL
 *
 * Programmer:	Robb Matzke (copied from H5Sset_extent_simple)
 *              Wednesday, July  8, 1998
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5S_set_extent_simple (H5S_t *space, int rank, const hsize_t *dims,
		       const hsize_t *max)
{
    FUNC_ENTER(H5S_set_extent_simple, FAIL);

    /* Check args */
    assert(rank>=0);
    assert(0==rank || dims);
    
    /* shift out of the previous state to a "simple" dataspace */
    switch (space->extent.type) {
        case H5S_SCALAR:
            /* do nothing */
            break;

        case H5S_SIMPLE:
            H5S_release_simple(&(space->extent.u.simple));
            break;

        case H5S_COMPLEX:
        /*
         * eventually this will destroy whatever "complex" dataspace info
         * is retained, right now it's an error
         */
        /* Fall through to report error */

        default:
            HRETURN_ERROR(H5E_DATASPACE, H5E_BADVALUE, FAIL,
                  "unknown data space class");
    }

    if (rank == 0) {		/* scalar variable */
        space->extent.type = H5S_SCALAR;
        space->extent.u.simple.rank = 0;	/* set to scalar rank */
    } else {
        space->extent.type = H5S_SIMPLE;

        /* Set the rank and copy the dims */
        space->extent.u.simple.rank = rank;
        space->extent.u.simple.size = H5MM_malloc(rank*sizeof(hsize_t));
        HDmemcpy(space->extent.u.simple.size, dims, sizeof(hsize_t) * rank);

        /* Copy the maximum dimensions if specified */
        if(max!=NULL) {
            space->extent.u.simple.max = H5MM_malloc(rank*sizeof(hsize_t));
            HDmemcpy(space->extent.u.simple.max, max, sizeof(hsize_t) * rank);
        } /* end if */
    }
    FUNC_LEAVE(SUCCEED);
}


/*-------------------------------------------------------------------------
 * Function:	H5S_find
 *
 * Purpose:	Given two data spaces (MEM_SPACE and FILE_SPACE) this
 *		function locates the data space conversion functions and
 *		initializes CONV to point to them.  The CONV contains
 *		function pointers for converting in either direction.
 *
 * Return:	Success:	Pointer to a data space conversion callback
 *				list.
 *
 *		Failure:	NULL
 *
 * Programmer:	Robb Matzke
 *		Wednesday, January 21, 1998
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
herr_t
H5S_find (H5S_conv_t *conv, const H5S_t *mem_space, const H5S_t *file_space)
{
    FUNC_ENTER (H5S_find, FAIL);

    /* Check args */
    assert (conv);
    assert (mem_space && (H5S_SIMPLE==mem_space->extent.type || H5S_SCALAR==mem_space->extent.type));
    assert (file_space && (H5S_SIMPLE==file_space->extent.type || H5S_SCALAR==mem_space->extent.type));

    /*
     * We can't do conversion if the source and destination select a
     * different number of data points.
     */
    if (H5S_select_npoints (mem_space) != H5S_select_npoints (file_space)) {
        HRETURN_ERROR (H5E_DATASPACE, H5E_BADRANGE, FAIL,
		       "memory and file data spaces are different sizes");
    }

#ifdef OLD_WAY
    /*
     * Initialize pointers.  This will eventually be a table lookup based
     * on the source and destination data spaces, similar to H5T_find(), but
     * for now we only support simple data spaces.
     */
    if (!conv) {
        _conv.init = H5S_simp_init;
        _conv.fgath = H5S_simp_fgath;
        _conv.mscat = H5S_simp_mscat;
        _conv.mgath = H5S_simp_mgath;
        _conv.fscat = H5S_simp_fscat;
        _conv.read = H5S_simp_read;
        _conv.write = H5S_simp_write;
        conv = &_conv;
    }
#else
    /* Set up the function pointers for file transfers */
    switch(file_space->select.type) {
        case H5S_SEL_POINTS:
#ifdef QAK
printf("%s: file space has point selection\n",FUNC);
#endif /* QAK */
            conv->finit = H5S_point_init;
            conv->favail = H5S_point_favail;
            conv->fgath = H5S_point_fgath;
            conv->fscat = H5S_point_fscat;
            conv->read = NULL;
            conv->write = NULL;
            break;

        case H5S_SEL_ALL:
#ifdef QAK
printf("%s: file space has all selection\n",FUNC);
#endif /* QAK */
            conv->finit = H5S_all_init;
            conv->favail = H5S_all_favail;
            conv->fgath = H5S_all_fgath;
            conv->fscat = H5S_all_fscat;
            conv->read = NULL;
            conv->write = NULL;
            break;

        case H5S_SEL_HYPERSLABS:
#ifdef QAK
printf("%s: file space has hyperslab selection\n",FUNC);
#endif /* QAK */
            conv->finit = H5S_hyper_init;
            conv->favail = H5S_hyper_favail;
            conv->fgath = H5S_hyper_fgath;
            conv->fscat = H5S_hyper_fscat;
            conv->read = NULL;
            conv->write = NULL;
            break;

        case H5S_SEL_NONE:
        default:
#ifdef QAK
printf("%s: file space has unknown selection\n",FUNC);
#endif /* QAK */
            HRETURN_ERROR (H5E_DATASPACE, H5E_BADVALUE, FAIL,
                   "invalid file dataspace selection type");
    } /* end switch */

    /* Set up the function pointers for background & memory transfers */
    switch(mem_space->select.type) {
        case H5S_SEL_POINTS:
#ifdef QAK
printf("%s: memory space has point selection\n",FUNC);
#endif /* QAK */
            conv->minit = H5S_point_init;
            conv->binit = H5S_point_init;
            conv->mgath = H5S_point_mgath;
            conv->mscat = H5S_point_mscat;
            conv->read = NULL;
            conv->write = NULL;
            break;

        case H5S_SEL_ALL:
#ifdef QAK
printf("%s: memory space has all selection\n",FUNC);
#endif /* QAK */
            conv->minit = H5S_all_init;
            conv->binit = H5S_all_init;
            conv->mgath = H5S_all_mgath;
            conv->mscat = H5S_all_mscat;
            conv->read = NULL;
            conv->write = NULL;
            break;

        case H5S_SEL_HYPERSLABS:
#ifdef QAK
printf("%s: memory space has hyperslab selection\n",FUNC);
#endif /* QAK */
            conv->minit = H5S_hyper_init;
            conv->binit = H5S_hyper_init;
            conv->mgath = H5S_hyper_mgath;
            conv->mscat = H5S_hyper_mscat;
            conv->read = NULL;
            conv->write = NULL;
            break;

        case H5S_SEL_NONE:
        default:
#ifdef QAK
printf("%s: memory space has unknown selection\n",FUNC);
#endif /* QAK */
            HRETURN_ERROR (H5E_DATASPACE, H5E_BADVALUE, FAIL,
                   "invalid file dataspace selection type");
    } /* end switch */
#endif /* OLD_WAY */
    
    FUNC_LEAVE (SUCCEED);
}

/*-------------------------------------------------------------------------
 * Function:	H5S_extend
 *
 * Purpose:	Extend the dimensions of a data space.
 *
 * Return:	Success:	Number of dimensions whose size increased.
 *
 *		Failure:	FAIL
 *
 * Programmer:	Robb Matzke
 *		Friday, January 30, 1998
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
intn
H5S_extend (H5S_t *space, const hsize_t *size)
{
    intn		i, ret_value=0;
    
    FUNC_ENTER (H5S_extend, FAIL);

    /* Check args */
    assert (space && H5S_SIMPLE==space->extent.type);
    assert (size);

    for (i=0; i<space->extent.u.simple.rank; i++) {
        if (space->extent.u.simple.size[i]<size[i]) {
            if (space->extent.u.simple.max &&
                    H5S_UNLIMITED!=space->extent.u.simple.max[i] &&
                    space->extent.u.simple.max[i]<size[i]) {
                HRETURN_ERROR (H5E_ARGS, H5E_BADVALUE, FAIL,
                       "dimension cannot be increased");
            }
            ret_value++;
        }
    }

    /* Update */
    if (ret_value) {
        for (i=0; i<space->extent.u.simple.rank; i++) {
            if (space->extent.u.simple.size[i]<size[i]) {
                space->extent.u.simple.size[i] = size[i];
            }
        }
    }

    FUNC_LEAVE (ret_value);
}

/*-------------------------------------------------------------------------
 * Function:	H5Screate_simple
 *
 * Purpose:	Creates a new simple data space object and opens it for
 *		access. The DIMS argument is the size of the simple dataset
 *		and the MAXDIMS argument is the upper limit on the size of
 *		the dataset.  MAXDIMS may be the null pointer in which case
 *		the upper limit is the same as DIMS.  If an element of
 *		MAXDIMS is H5S_UNLIMITED then the corresponding dimension is
 *		unlimited, otherwise no element of MAXDIMS should be smaller
 *		than the corresponding element of DIMS.
 *
 * Return:	Success:	The ID for the new simple data space object.
 *
 *		Failure:	FAIL
 *
 * Errors:
 *
 * Programmer:	Quincey Koziol
 *		Tuesday, January  27, 1998
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
hid_t
H5Screate_simple (int rank, const hsize_t *dims, const hsize_t *maxdims)
{
    hid_t	ret_value = FAIL;
    H5S_t	*space = NULL;
    int		i;

    FUNC_ENTER(H5Screate, FAIL);
    H5TRACE3("i","Is*h*h",rank,dims,maxdims);

    /* Check arguments */
    if (rank<0) {
        HRETURN_ERROR (H5E_ARGS, H5E_BADVALUE, FAIL,
		       "dimensionality cannot be negative");
    }
    if (!dims && dims!=0) {
        HRETURN_ERROR (H5E_ARGS, H5E_BADVALUE, FAIL,
		       "no dimensions specified");
    }
    if (maxdims) {
        for (i=0; i<rank; i++) {
            if (H5S_UNLIMITED!=maxdims[i] && maxdims[i]<dims[i]) {
                HRETURN_ERROR (H5E_ARGS, H5E_BADVALUE, FAIL,
                       "maxdims is smaller than dims");
            }
        }
    }

    /* Create the space and set the extent */
    if(NULL==(space=H5S_create(H5S_SIMPLE))) {
        HGOTO_ERROR (H5E_DATASPACE, H5E_CANTCREATE, FAIL,
		     "can't create simple dataspace");
    }
    if(H5S_set_extent_simple(space,rank,dims,maxdims)<0) {
        HGOTO_ERROR (H5E_DATASPACE, H5E_CANTINIT, FAIL,
		     "can't set dimensions");
    }
    
    /* Atomize */
    if ((ret_value=H5I_register (H5_DATASPACE, space))<0) {
        HGOTO_ERROR (H5E_ATOM, H5E_CANTREGISTER, FAIL,
		     "unable to register data space atom");
    }
    
 done:
    if (ret_value<0 && space) H5S_close(space);
    FUNC_LEAVE(ret_value);
}
