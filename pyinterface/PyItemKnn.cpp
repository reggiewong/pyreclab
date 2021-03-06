#include "PyItemKnn.h"
#include "AlgFactory.h"
#include "AlgItemBasedKnn.h"

#include <iostream>
#include <string>

using namespace std;


PyObject* ItemKnn_new( PyTypeObject* type, PyObject* args, PyObject* kwdict )
{
   const char* dsfilename = NULL;
   char dlmchar = ',';
   int header = 0;
   int usercol = 0;
   int itemcol = 1;
   int ratingcol = 2;

   static char* kwlist[] = { const_cast<char*>( "dataset" ),
                             const_cast<char*>( "dlmchar" ),
                             const_cast<char*>( "header" ),
                             const_cast<char*>( "usercol" ),
                             const_cast<char*>( "itemcol" ),
                             const_cast<char*>( "ratingcol" ),
                             NULL };

   if( !PyArg_ParseTupleAndKeywords( args, kwdict, "s|ciiii", kwlist, &dsfilename,
                                     &dlmchar, &header, &usercol, &itemcol, &ratingcol ) )
   {
      return NULL;
   }

   if( NULL == dsfilename )
   {
      return NULL;
   }

   Recommender* self = reinterpret_cast<Recommender*>( type->tp_alloc( type, 0 ) );
   if( self != NULL )
   {
      self->m_trainingReader = new DataReader( dsfilename, dlmchar, header );
      if( NULL == self->m_trainingReader )
      {
         Py_DECREF( self );
         return NULL;
      }

      self->m_ratingMatrix = new RatingMatrix( *(self->m_trainingReader), usercol, itemcol, ratingcol );
      if( NULL == self->m_ratingMatrix )
      {
         Py_DECREF( self );
         return NULL;
      }

      self->m_recAlgorithm = AlgFactory::getInstance( "ItemKnn", *(self->m_ratingMatrix) );
      if( NULL == self->m_recAlgorithm )
      {
         Py_DECREF( self );
         return NULL;
      }
   }

   return reinterpret_cast<PyObject*>( self );
}

PyObject* ItemKnn_train( Recommender* self, PyObject* args, PyObject* kwdict )
{  
   int k = 10;

   static char* kwlist[] = { const_cast<char*>( "k" ),
                             NULL };

   if( !PyArg_ParseTupleAndKeywords( args, kwdict, "|i", kwlist, &k ) )
   {
      return NULL;
   }

   dynamic_cast<AlgItemBasedKnn*>( self->m_recAlgorithm )->train( k );
   Py_INCREF( Py_None );
   return Py_None;
}


