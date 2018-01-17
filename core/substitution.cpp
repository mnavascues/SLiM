//
//  substitution.cpp
//  SLiM
//
//  Created by Ben Haller on 12/13/14.
//  Copyright (c) 2014-2017 Philipp Messer.  All rights reserved.
//	A product of the Messer Lab, http://messerlab.org/slim/
//

//	This file is part of SLiM.
//
//	SLiM is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
//	SLiM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License along with SLiM.  If not, see <http://www.gnu.org/licenses/>.


#include "substitution.h"
#include "slim_global.h"
#include "eidos_call_signature.h"
#include "eidos_property_signature.h"

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>


#pragma mark -
#pragma mark Substitution
#pragma mark -

Substitution::Substitution(Mutation &p_mutation, slim_generation_t p_fixation_generation) :
mutation_type_ptr_(p_mutation.mutation_type_ptr_), position_(p_mutation.position_), selection_coeff_(p_mutation.selection_coeff_), subpop_index_(p_mutation.subpop_index_), generation_(p_mutation.generation_), fixation_generation_(p_fixation_generation), mutation_id_(p_mutation.mutation_id_), tag_value_(p_mutation.tag_value_)
{
}

void Substitution::PrintForSLiMOutput(std::ostream &p_out) const
{ 
	p_out << mutation_id_ << " m" << mutation_type_ptr_->mutation_type_id_ << " " << position_ << " " << selection_coeff_ << " " << mutation_type_ptr_->dominance_coeff_ << " p" << subpop_index_ << " " << generation_ << " "<< fixation_generation_ << std::endl;
}


//
//	Eidos support
//
#pragma mark -
#pragma mark Eidos support
#pragma mark -

const EidosObjectClass *Substitution::Class(void) const
{
	return gSLiM_Substitution_Class;
}

void Substitution::Print(std::ostream &p_ostream) const
{
	p_ostream << Class()->ElementType() << "<" << mutation_id_ << ":" << selection_coeff_ << ">";
}

EidosValue_SP Substitution::GetProperty(EidosGlobalStringID p_property_id)
{
	// All of our strings are in the global registry, so we can require a successful lookup
	switch (p_property_id)
	{
			// constants
		case gID_id:					// ACCELERATED
			return EidosValue_SP(new (gEidosValuePool->AllocateChunk()) EidosValue_Int_singleton(mutation_id_));
		case gID_mutationType:			// ACCELERATED
			return mutation_type_ptr_->SymbolTableEntry().second;
		case gID_position:				// ACCELERATED
			return EidosValue_SP(new (gEidosValuePool->AllocateChunk()) EidosValue_Int_singleton(position_));
		case gID_selectionCoeff:		// ACCELERATED
			return EidosValue_SP(new (gEidosValuePool->AllocateChunk()) EidosValue_Float_singleton(selection_coeff_));
		case gID_subpopID:				// ACCELERATED
			return EidosValue_SP(new (gEidosValuePool->AllocateChunk()) EidosValue_Int_singleton(subpop_index_));
		case gID_originGeneration:		// ACCELERATED
			return EidosValue_SP(new (gEidosValuePool->AllocateChunk()) EidosValue_Int_singleton(generation_));
		case gID_fixationGeneration:	// ACCELERATED
			return EidosValue_SP(new (gEidosValuePool->AllocateChunk()) EidosValue_Int_singleton(fixation_generation_));
			
			// variables
		case gID_tag:					// ACCELERATED
			return EidosValue_SP(new (gEidosValuePool->AllocateChunk()) EidosValue_Int_singleton(tag_value_));
			
			// all others, including gID_none
		default:
			return EidosObjectElement::GetProperty(p_property_id);
	}
}

EidosValue *Substitution::GetProperty_Accelerated_id(EidosObjectElement **p_values, size_t p_values_size)
{
	EidosValue_Int_vector *int_result = (new (gEidosValuePool->AllocateChunk()) EidosValue_Int_vector())->resize_no_initialize(p_values_size);
	
	for (size_t value_index = 0; value_index < p_values_size; ++value_index)
	{
		Substitution *value = (Substitution *)(p_values[value_index]);
		
		int_result->set_int_no_check(value->mutation_id_, value_index);
	}
	
	return int_result;
}

EidosValue *Substitution::GetProperty_Accelerated_originGeneration(EidosObjectElement **p_values, size_t p_values_size)
{
	EidosValue_Int_vector *int_result = (new (gEidosValuePool->AllocateChunk()) EidosValue_Int_vector())->resize_no_initialize(p_values_size);
	
	for (size_t value_index = 0; value_index < p_values_size; ++value_index)
	{
		Substitution *value = (Substitution *)(p_values[value_index]);
		
		int_result->set_int_no_check(value->generation_, value_index);
	}
	
	return int_result;
}

EidosValue *Substitution::GetProperty_Accelerated_fixationGeneration(EidosObjectElement **p_values, size_t p_values_size)
{
	EidosValue_Int_vector *int_result = (new (gEidosValuePool->AllocateChunk()) EidosValue_Int_vector())->resize_no_initialize(p_values_size);
	
	for (size_t value_index = 0; value_index < p_values_size; ++value_index)
	{
		Substitution *value = (Substitution *)(p_values[value_index]);
		
		int_result->set_int_no_check(value->fixation_generation_, value_index);
	}
	
	return int_result;
}

EidosValue *Substitution::GetProperty_Accelerated_position(EidosObjectElement **p_values, size_t p_values_size)
{
	EidosValue_Int_vector *int_result = (new (gEidosValuePool->AllocateChunk()) EidosValue_Int_vector())->resize_no_initialize(p_values_size);
	
	for (size_t value_index = 0; value_index < p_values_size; ++value_index)
	{
		Substitution *value = (Substitution *)(p_values[value_index]);
		
		int_result->set_int_no_check(value->position_, value_index);
	}
	
	return int_result;
}

EidosValue *Substitution::GetProperty_Accelerated_subpopID(EidosObjectElement **p_values, size_t p_values_size)
{
	EidosValue_Int_vector *int_result = (new (gEidosValuePool->AllocateChunk()) EidosValue_Int_vector())->resize_no_initialize(p_values_size);
	
	for (size_t value_index = 0; value_index < p_values_size; ++value_index)
	{
		Substitution *value = (Substitution *)(p_values[value_index]);
		
		int_result->set_int_no_check(value->subpop_index_, value_index);
	}
	
	return int_result;
}

EidosValue *Substitution::GetProperty_Accelerated_tag(EidosObjectElement **p_values, size_t p_values_size)
{
	EidosValue_Int_vector *int_result = (new (gEidosValuePool->AllocateChunk()) EidosValue_Int_vector())->resize_no_initialize(p_values_size);
	
	for (size_t value_index = 0; value_index < p_values_size; ++value_index)
	{
		Substitution *value = (Substitution *)(p_values[value_index]);
		
		int_result->set_int_no_check(value->tag_value_, value_index);
	}
	
	return int_result;
}

EidosValue *Substitution::GetProperty_Accelerated_selectionCoeff(EidosObjectElement **p_values, size_t p_values_size)
{
	EidosValue_Float_vector *float_result = (new (gEidosValuePool->AllocateChunk()) EidosValue_Float_vector())->resize_no_initialize(p_values_size);
	
	for (size_t value_index = 0; value_index < p_values_size; ++value_index)
	{
		Substitution *value = (Substitution *)(p_values[value_index]);
		
		float_result->set_float_no_check(value->selection_coeff_, value_index);
	}
	
	return float_result;
}

EidosValue *Substitution::GetProperty_Accelerated_mutationType(EidosObjectElement **p_values, size_t p_values_size)
{
	EidosValue_Object_vector *object_result = (new (gEidosValuePool->AllocateChunk()) EidosValue_Object_vector(gSLiM_MutationType_Class))->resize_no_initialize(p_values_size);
	
	for (size_t value_index = 0; value_index < p_values_size; ++value_index)
	{
		Substitution *value = (Substitution *)(p_values[value_index]);
		
		object_result->set_object_element_no_check(value->mutation_type_ptr_, value_index);
	}
	
	return object_result;
}

void Substitution::SetProperty(EidosGlobalStringID p_property_id, const EidosValue &p_value)
{
	// All of our strings are in the global registry, so we can require a successful lookup
	switch (p_property_id)
	{
		case gID_subpopID:
		{
			slim_objectid_t value = SLiMCastToObjectidTypeOrRaise(p_value.IntAtIndex(0, nullptr));
			
			subpop_index_ = value;
			return;
		}
		case gID_tag:
		{
			slim_usertag_t value = SLiMCastToUsertagTypeOrRaise(p_value.IntAtIndex(0, nullptr));
			
			tag_value_ = value;
			return;
		}
		
		default:
		{
			return EidosObjectElement::SetProperty(p_property_id, p_value);
		}
	}
}


//
//	Substitution_Class
//
#pragma mark -
#pragma mark Substitution_Class
#pragma mark -

class Substitution_Class : public EidosObjectClass
{
public:
	Substitution_Class(const Substitution_Class &p_original) = delete;	// no copy-construct
	Substitution_Class& operator=(const Substitution_Class&) = delete;	// no copying
	inline Substitution_Class(void) { }
	
	virtual const std::string &ElementType(void) const;
	
	virtual const std::vector<const EidosPropertySignature *> *Properties(void) const;
};

EidosObjectClass *gSLiM_Substitution_Class = new Substitution_Class();


const std::string &Substitution_Class::ElementType(void) const
{
	return gStr_Substitution;
}

const std::vector<const EidosPropertySignature *> *Substitution_Class::Properties(void) const
{
	static std::vector<const EidosPropertySignature *> *properties = nullptr;
	
	if (!properties)
	{
		properties = new std::vector<const EidosPropertySignature *>(*EidosObjectClass::Properties());
		
		properties->emplace_back((EidosPropertySignature *)(new EidosPropertySignature(gStr_id,					true,	kEidosValueMaskInt | kEidosValueMaskSingleton))->DeclareAcceleratedGet(Substitution::GetProperty_Accelerated_id));
		properties->emplace_back((EidosPropertySignature *)(new EidosPropertySignature(gStr_mutationType,		true,	kEidosValueMaskObject | kEidosValueMaskSingleton, gSLiM_MutationType_Class))->DeclareAcceleratedGet(Substitution::GetProperty_Accelerated_mutationType));
		properties->emplace_back((EidosPropertySignature *)(new EidosPropertySignature(gStr_position,			true,	kEidosValueMaskInt | kEidosValueMaskSingleton))->DeclareAcceleratedGet(Substitution::GetProperty_Accelerated_position));
		properties->emplace_back((EidosPropertySignature *)(new EidosPropertySignature(gStr_selectionCoeff,		true,	kEidosValueMaskFloat | kEidosValueMaskSingleton))->DeclareAcceleratedGet(Substitution::GetProperty_Accelerated_selectionCoeff));
		properties->emplace_back((EidosPropertySignature *)(new EidosPropertySignature(gStr_subpopID,			false,	kEidosValueMaskInt | kEidosValueMaskSingleton))->DeclareAcceleratedGet(Substitution::GetProperty_Accelerated_subpopID));
		properties->emplace_back((EidosPropertySignature *)(new EidosPropertySignature(gStr_originGeneration,	true,	kEidosValueMaskInt | kEidosValueMaskSingleton))->DeclareAcceleratedGet(Substitution::GetProperty_Accelerated_originGeneration));
		properties->emplace_back((EidosPropertySignature *)(new EidosPropertySignature(gStr_fixationGeneration,	true,	kEidosValueMaskInt | kEidosValueMaskSingleton))->DeclareAcceleratedGet(Substitution::GetProperty_Accelerated_fixationGeneration));
		properties->emplace_back((EidosPropertySignature *)(new EidosPropertySignature(gStr_tag,				false,	kEidosValueMaskInt | kEidosValueMaskSingleton))->DeclareAcceleratedGet(Substitution::GetProperty_Accelerated_tag));
		
		std::sort(properties->begin(), properties->end(), CompareEidosPropertySignatures);
	}
	
	return properties;
}







































































