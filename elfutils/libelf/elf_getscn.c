/* Get section at specific index.
   Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
   Contributed by Ulrich Drepper <drepper@redhat.com>, 1998.

   This program is Open Source software; you can redistribute it and/or
   modify it under the terms of the Open Software License version 1.0 as
   published by the Open Source Initiative.

   You should have received a copy of the Open Software License along
   with this program; if not, you may obtain a copy of the Open Software
   License version 1.0 from http://www.opensource.org/license/osl.php or
   by writing the Open Source Initiative c/o Lawrence Rosen, Esq.,
   3001 King Ranch Road, Ukiah, CA 95482.   */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "libelfP.h"


Elf_Scn *
elf_getscn (Elf *elf, size_t index)
{
  Elf_Scn *result = NULL;
  Elf_ScnList *runp;

  if (elf == NULL)
    return NULL;

  if (unlikely (elf->kind != ELF_K_ELF))
    {
      __libelf_seterrno (ELF_E_INVALID_HANDLE);
      return NULL;
    }

  rwlock_rdlock (elf->lock);

  /* Find the section in the list.  */
  runp = (elf->class == ELFCLASS32
	  || (offsetof (struct Elf, state.elf32.scns)
	      == offsetof (struct Elf, state.elf64.scns))
	  ? &elf->state.elf32.scns : &elf->state.elf64.scns);
  while (1)
    {
      if (index < runp->max)
	{
	  if (index < runp->cnt)
	    result = &runp->data[index];
	  else
	    __libelf_seterrno (ELF_E_INVALID_INDEX);
	  break;
	}

      index -= runp->max;

      runp = runp->next;
      if (runp == NULL)
	{
	  __libelf_seterrno (ELF_E_INVALID_INDEX);
	  break;
	}
    }

  rwlock_unlock (elf->lock);

  return result;
}
INTDEF(elf_getscn)
