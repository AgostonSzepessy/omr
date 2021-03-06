/******************************************************************************
 Copyright IBM Corp. 2017, 2017
 *
 *  This program and the accompanying materials are made available
 *  under the terms of the Eclipse Public License v1.0 and
 *  Apache License v2.0 which accompanies this distribution.
 *
 *      The Eclipse Public License is available at
 *      http://www.eclipse.org/legal/epl-v10.html
 *
 *      The Apache License v2.0 is available at
 *      http://www.opensource.org/licenses/apache2.0.php
 *
 * Contributors:
 *    Multiple authors (IBM Corp.) - initial implementation and documentation
 ******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dlfcn.h>
#include <errno.h>
#include <assert.h>

#include "Jit.hpp"
#include "ilgen/TypeDictionary.hpp"
#include "ilgen/MethodBuilder.hpp"
#include "FieldAddress.hpp"


GetStructFieldAddressBuilder::GetStructFieldAddressBuilder(TR::TypeDictionary *d)
   : MethodBuilder(d)
   {
   DefineLine(LINETOSTR(__LINE__));
   DefineFile(__FILE__);

   pStructType = d->PointerTo(d->LookupStruct("Struct"));

   DefineName("getStructFieldAddress");
   DefineParameter("s", pStructType);
   DefineReturnType(d->PointerTo(d->GetFieldType("Struct", "f2")));
   }

bool
GetStructFieldAddressBuilder::buildIL()
   {
   Return(
          StructFieldInstanceAddress("Struct", "f2",
                             Load("s")));

   return true;
   }

GetUnionFieldAddressBuilder::GetUnionFieldAddressBuilder(TR::TypeDictionary *d)
   : MethodBuilder(d)
   {
   DefineLine(LINETOSTR(__LINE__));
   DefineFile(__FILE__);

   pUnionType = d->PointerTo(d->LookupUnion("Union"));

   DefineName("getUnionFieldAddress");
   DefineParameter("u", pUnionType);
   DefineReturnType(d->toIlType<uint8_t *>());
   }

bool
GetUnionFieldAddressBuilder::buildIL()
   {
   Return(
          UnionFieldInstanceAddress("Union", "f2",
                             Load("u")));

   return true;
   }

class StructTypeDictionary : public TR::TypeDictionary
   {
   public:
   StructTypeDictionary()
      : TR::TypeDictionary()
      {
      DEFINE_STRUCT(Struct);
      DEFINE_FIELD(Struct, f1, toIlType<uint16_t>());
      DEFINE_FIELD(Struct, f2, toIlType<uint8_t>());
      CLOSE_STRUCT(Struct);
      }
   };

class UnionTypeDictionary : public TR::TypeDictionary
   {
   public:
   UnionTypeDictionary()
      : TR::TypeDictionary()
      {
      DefineUnion("Union");
      UnionField("Union", "f1", toIlType<uint16_t>());
      UnionField("Union", "f2", toIlType<uint8_t>());
      CloseUnion("Union");
      }
   };


int
main(int argc, char *argv[])
   {
   printf("Step 1: initialize JIT\n");
   bool initialized = initializeJit();
   if (!initialized)
      {
      fprintf(stderr, "FAIL: could not initialize JIT\n");
      exit(-1);
      }

   printf("Step 2: define type dictionaries\n");
   StructTypeDictionary structTypes;
   UnionTypeDictionary unionTypes;

   printf("Step 3: compile getStructFieldAddress builder\n");
   GetStructFieldAddressBuilder getStructFieldAddressBuilder(&structTypes);
   uint8_t *getStructFieldAddressEntry;
   int32_t rc = compileMethodBuilder(&getStructFieldAddressBuilder, &getStructFieldAddressEntry);
   if (rc != 0)
      {
      fprintf(stderr,"FAIL: compilation error %d\n", rc);
      exit(-2);
      }

   printf("Step 4: invoke compiled code for getStructFieldAddress and verify results\n");
   Struct s;
   s.f1 = 1;
   s.f2 = 2;
   auto getStructFieldAddress = (GetStructFieldAddressFunction *) getStructFieldAddressEntry;
   auto structFieldAddress = getStructFieldAddress(&s);
   assert(&(s.f2) == structFieldAddress);

   printf("Step 5: compile getUnionFieldAddress builder\n");
   GetUnionFieldAddressBuilder getUnionFieldAddressBuilder(&unionTypes);
   uint8_t *getUnionFieldAddressEntry;
   rc = compileMethodBuilder(&getUnionFieldAddressBuilder, &getUnionFieldAddressEntry);
   if (rc != 0)
      {
      fprintf(stderr,"FAIL: compilation error %d\n", rc);
      exit(-2);
      }

   printf("Step 6: invoke compiled code for getUnionFieldAddress and verify results\n");
   Union u;
   u.f2 = 2;
   auto getUnionFieldAddress = (GetUnionFieldAddressFunction  *) getUnionFieldAddressEntry;
   auto unionFieldAddress = getUnionFieldAddress(&u);
   assert(&(u.f2) == unionFieldAddress);

   printf ("Step 7: shutdown JIT\n");
   shutdownJit();

   printf("PASS\n");
   }
