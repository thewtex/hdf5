/****************************************************************************
 * NCSA HDF								    *
 * Software Development Group						    *
 * National Center for Supercomputing Applications			    *
 * University of Illinois at Urbana-Champaign				    *
 * 605 E. Springfield, Champaign IL 61820				    *
 *									    *
 * For conditions of distribution and use, see the accompanying		    *
 * hdf/COPYING file.							    *
 *									    *
 ****************************************************************************/

/*
 * Open object info algorithms.
 *
 * These are used to track the objects currently open in a file, for various
 * internal mechanisms which need to be aware of such things.
 *
 */

#define H5F_PACKAGE		/*suppress error about including H5Fpkg	  */

#include "H5Eprivate.h"		/* Error handling		  	*/
#include "H5Fpkg.h"             /* File access                          */
#include "H5FLprivate.h"	/* Free lists                           */
#include "H5FOprivate.h"        /* File objects                         */

#define PABLO_MASK	H5FO_mask

/* Interface initialization */
static int		interface_initialize_g = 0;
#define INTERFACE_INIT	NULL

/* Private typedefs */

/* Information about object objects in a file */
typedef struct H5FO_open_obj_t {
    haddr_t addr;                       /* Address of object header for object */
                                        /* THIS MUST BE FIRST FOR TBBT ROUTINES */
    hid_t id;                           /* Current ID for object            */
} H5FO_open_obj_t;

/* Declare a free list to manage the H5FO_open_obj_t struct */
H5FL_DEFINE_STATIC(H5FO_open_obj_t);


/*--------------------------------------------------------------------------
 NAME
    H5FO_create
 PURPOSE
    Create an open object info set
 USAGE
    herr_t H5FO_create(f)
        H5F_t *f;       IN/OUT: File to create opened object info set for

 RETURNS
    Returns non-negative on success, negative on failure
 DESCRIPTION
    Create a new open object info set.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
herr_t
H5FO_create(H5F_t *f)
{
    herr_t ret_value=SUCCEED;          /* Return value */

    FUNC_ENTER_NOAPI(H5FO_create,FAIL);

    /* Sanity check */
    assert(f);
    assert(f->shared);

    /* Create TBBT used to store open object info */
    if((f->shared->open_objs=H5TB_fast_dmake(H5TB_FAST_HADDR_COMPARE))==NULL)
        HGOTO_ERROR(H5E_FILE, H5E_CANTINIT, FAIL, "unable to create open object TBBT");

done:
    FUNC_LEAVE(ret_value);
} /* end H5FO_create() */


/*--------------------------------------------------------------------------
 NAME
    H5FO_opened
 PURPOSE
    Checks if an object at an address is already open in the file.
 USAGE
    hid_t H5FO_opened(f,addr)
        const H5F_t *f;         IN: File to check opened object info set
        haddr_t addr;           IN: Address of object to check

 RETURNS
    Returns a non-negative ID for the object on success, negative on failure
 DESCRIPTION
    Check is an object at an address (the address of the object's object header)
    is already open in the file and return the ID for that object if it is open.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
hid_t
H5FO_opened(const H5F_t *f, haddr_t addr)
{
    H5TB_NODE *obj_node;        /* TBBT node holding open object */
    H5FO_open_obj_t *open_obj;  /* Information about open object */
    hid_t ret_value;            /* Return value */

    FUNC_ENTER_NOAPI(H5FO_opened,FAIL);

    /* Sanity check */
    assert(f);
    assert(f->shared);
    assert(f->shared->open_objs);
    assert(H5F_addr_defined(addr));

    /* Get the object node from the TBBT */
    if((obj_node=H5TB_dfind(f->shared->open_objs,&addr,NULL))!=NULL) {
        open_obj=H5TB_NODE_DATA(obj_node);
        assert(open_obj);
        ret_value=open_obj->id;
        assert(ret_value>0);
    } /* end if */
    else
        ret_value=FAIL;

done:
    FUNC_LEAVE(ret_value);
} /* end H5FO_opened() */


/*--------------------------------------------------------------------------
 NAME
    H5FO_insert
 PURPOSE
    Insert a newly opened object/ID pair into the opened object info set
 USAGE
    herr_t H5FO_insert(f,addr,id)
        H5F_t *f;               IN/OUT: File's opened object info set
        haddr_t addr;           IN: Address of object to insert
        hid_t id;               IN: ID of object to insert

 RETURNS
    Returns a non-negative on success, negative on failure
 DESCRIPTION
    Insert an object/ID pair into the opened object info set.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
herr_t
H5FO_insert(H5F_t *f, haddr_t addr, hid_t id)
{
    H5FO_open_obj_t *open_obj;  /* Information about open object */
    herr_t ret_value=SUCCEED;   /* Return value */

    FUNC_ENTER_NOAPI(H5FO_insert,FAIL);

    /* Sanity check */
    assert(f);
    assert(f->shared);
    assert(f->shared->open_objs);
    assert(H5F_addr_defined(addr));
    assert(id>0);

    /* Allocate new opened object information structure */
    if((open_obj=H5FL_MALLOC(H5FO_open_obj_t))==NULL)
        HGOTO_ERROR(H5E_CACHE,H5E_NOSPACE,FAIL,"memory allocation failed");

    /* Assign information */
    open_obj->addr=addr;
    open_obj->id=id;

    /* Insert into TBBT */
    if(H5TB_dins(f->shared->open_objs,open_obj,open_obj)==NULL)
        HGOTO_ERROR(H5E_CACHE,H5E_CANTINSERT,FAIL,"can't insert object into TBBT");

done:
    FUNC_LEAVE(ret_value);
} /* end H5FO_insert() */


/*--------------------------------------------------------------------------
 NAME
    H5FO_delete
 PURPOSE
    Remove an opened object/ID pair from the opened object info set
 USAGE
    herr_t H5FO_delete(f,addr)
        H5F_t *f;               IN/OUT: File's opened object info set
        haddr_t addr;           IN: Address of object to remove

 RETURNS
    Returns a non-negative on success, negative on failure
 DESCRIPTION
    Remove an object/ID pair from the opened object info.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
herr_t
H5FO_delete(H5F_t *f, haddr_t addr)
{
    H5TB_NODE *obj_node;        /* TBBT node holding open object */
    H5FO_open_obj_t *open_obj;  /* Information about open object */
    herr_t ret_value=SUCCEED;   /* Return value */

    FUNC_ENTER_NOAPI(H5FO_delete,FAIL);

    /* Sanity check */
    assert(f);
    assert(f->shared);
    assert(f->shared->open_objs);
    assert(H5F_addr_defined(addr));

    /* Get the object node from the TBBT */
    if((obj_node=H5TB_dfind(f->shared->open_objs,&addr,NULL))==NULL)
        HGOTO_ERROR(H5E_CACHE,H5E_NOTFOUND,FAIL,"can't locate object in TBBT");

    /* Remove from TBBT */
    if((open_obj=H5TB_rem(&f->shared->open_objs->root,obj_node,NULL))==NULL)
        HGOTO_ERROR(H5E_CACHE,H5E_CANTRELEASE,FAIL,"can't remove object from TBBT");

    /* Release the object information */
    H5FL_FREE(H5FO_open_obj_t,open_obj);

done:
    FUNC_LEAVE(ret_value);
} /* end H5FO_delete() */


/*--------------------------------------------------------------------------
 NAME
    H5FO_dest
 PURPOSE
    Destroy an open object info set
 USAGE
    herr_t H5FO_create(f)
        H5F_t *f;               IN/OUT: File's opened object info set

 RETURNS
    Returns a non-negative on success, negative on failure
 DESCRIPTION
    Destroy an existing open object info set.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
herr_t
H5FO_dest(H5F_t *f)
{
    herr_t ret_value=SUCCEED;   /* Return value */

    FUNC_ENTER_NOAPI(H5FO_dest,FAIL);

    /* Sanity check */
    assert(f);
    assert(f->shared);
    assert(f->shared->open_objs);

    /* Check if the object info set is empty */
    if(H5TB_count(f->shared->open_objs)!=0)
        HGOTO_ERROR(H5E_CACHE, H5E_CANTRELEASE, FAIL, "objects still in open object info set");

    /* Release the open object info set TBBT */
    f->shared->open_objs=H5TB_dfree(f->shared->open_objs,NULL,NULL);

done:
    FUNC_LEAVE(ret_value);
} /* end H5FO_dest() */

