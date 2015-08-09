//
//  subpopulation.h
//  SLiM
//
//  Created by Ben Haller on 12/13/14.
//  Copyright (c) 2014 Philipp Messer.  All rights reserved.
//	A product of the Messer Lab, http://messerlab.org/software/
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

/*
 
 The class Subpopulation represents one simulated subpopulation, defined primarily by the genomes of the individuals it contains.
 Since one Genome object represents the mutations along one chromosome, and since SLiM presently simulates diploid individuals,
 each individual is represented by two genomes in the genome vector: individual i is represented by genomes 2*i and 2*i+1.
 A subpopulations also knows its size, its selfing fraction, and what fraction it receives as migrants from other subpopulations.
 
 */

#ifndef __SLiM__subpopulation__
#define __SLiM__subpopulation__


#include <vector>

#include "slim_global.h"
#include "eidos_rng.h"
#include "genome.h"
#include "chromosome.h"
#include "eidos_value.h"
#include "slim_eidos_block.h"


class Population;


extern EidosObjectClass *gSLiM_Subpopulation_Class;


class Subpopulation : public EidosObjectElement
{
	//	This class has its copy constructor and assignment operator disabled, to prevent accidental copying.

private:
	
	gsl_ran_discrete_t *lookup_parent_ = nullptr;			// OWNED POINTER: lookup table for drawing a parent based upon fitness
	gsl_ran_discrete_t *lookup_female_parent_ = nullptr;	// OWNED POINTER: lookup table for drawing a female parent based upon fitness, SEX ONLY
	gsl_ran_discrete_t *lookup_male_parent_ = nullptr;		// OWNED POINTER: lookup table for drawing a male parent based upon fitness, SEX ONLY
	
	EidosSymbolTableEntry *self_symbol_ = nullptr;				// OWNED POINTER: EidosSymbolTableEntry object for fast setup of the symbol table
	
public:
	
	Population &population_;						// we need to know our Population so we can remove ourselves, etc.
	
	int subpopulation_id_;							// the id by which this subpopulation is indexed in the Population
	EidosValue *cached_value_subpop_id_ = nullptr;	// OWNED POINTER: a cached value for subpopulation_id_; delete and nil if changed
	
	double selfing_fraction_ = 0.0;					// ASEX ONLY: selfing fraction, the fraction of offspring generated by self-fertilization
	double female_clone_fraction_ = 0.0;			// both asex and sex; in the asex case, male_clone_fraction_ == female_clone_fraction_ and is simply the clonal fraction
	double male_clone_fraction_ = 0.0;				//		these represent the fraction of offspring generated by asexual clonal reproduction
	
	std::map<int,double> migrant_fractions_;		// m[i]: fraction made up of migrants from subpopulation i per generation
	bool child_generation_valid = false;			// this keeps track of whether children have been generated by EvolveSubpopulation() yet, or whether the parents are still in charge
	
	std::vector<Genome> parent_genomes_;			// all genomes in the parental generation; each individual gets two genomes, males are XY (not YX)
	int parent_subpop_size_;						// parental subpopulation size
	double parent_sex_ratio_ = 0.0;					// what sex ratio the parental genomes approximate
	int parent_first_male_index_ = INT_MAX;			// the index of the first male in the parental Genome vector (NOT premultiplied by 2!); equal to the number of females
	
	std::vector<Genome> child_genomes_;				// all genomes in the child generation; each individual gets two genomes, males are XY (not YX)
	int child_subpop_size_;							// child subpopulation size
	double child_sex_ratio_ = 0.0;					// what sex ratio the child genomes approximate
	int child_first_male_index_ = INT_MAX;			// the index of the first male in the child Genome vector (NOT premultiplied by 2!); equal to the number of females
	
	std::vector<SLiMEidosBlock*> registered_mate_choice_callbacks_;	// NOT OWNED: valid only during EvolveSubpopulation; callbacks used when this subpop is parental
	std::vector<SLiMEidosBlock*> registered_modify_child_callbacks_;	// NOT OWNED: valid only during EvolveSubpopulation; callbacks used when this subpop is parental
	
	double *cached_parental_fitness_ = nullptr;		// OWNED POINTER: cached in UpdateFitness(), used by SLiMgui and by the fitness() methods of Subpopulation
	double *cached_male_fitness_ = nullptr;			// OWNED POINTER: SEX ONLY: same as cached_parental_fitness_ but with 0 for all females
	int cached_fitness_size_ = 0;					// the size (number of entries used) of cached_parental_fitness_ and cached_male_fitness_
	int cached_fitness_capacity_ = 0;				// the capacity of the malloced buffers cached_parental_fitness_ and cached_male_fitness_
	
	// SEX ONLY; the default values here are for the non-sex case
	bool sex_enabled_ = false;										// the subpopulation needs to have easy reference to whether its individuals are sexual or not...
	GenomeType modeled_chromosome_type_ = GenomeType::kAutosome;	// ...and needs to know what type of chromosomes its individuals are modeling; this should match SLiMSim
	double x_chromosome_dominance_coeff_ = 1.0;						// the dominance coefficient for heterozygosity at the X locus (i.e. males); this is global
	
	int64_t tag_value_;																// a user-defined tag value
	
#ifdef SLIMGUI
	bool gui_selected_ = false;						// keeps track of whether we are selected in SLiMgui's table of subpopulations
	double parental_total_fitness_ = 0.0;			// updated in UpdateFitness() when running under SLiMgui
	double gui_center_x, gui_center_y, gui_radius;	// used as scratch space by GraphView_PopulationVisualization
#endif
	
	Subpopulation(const Subpopulation&) = delete;													// no copying
	Subpopulation& operator=(const Subpopulation&) = delete;										// no copying
	Subpopulation(void) = delete;																	// no null construction
	Subpopulation(Population &p_population, int p_subpopulation_id, int p_subpop_size);										// construct with a population size
	Subpopulation(Population &p_population, int p_subpopulation_id, int p_subpop_size, double p_sex_ratio,
				  GenomeType p_modeled_chromosome_type, double p_x_chromosome_dominance_coeff);		// SEX ONLY: construct with a sex ratio (fraction male), chromosome type (AXY), and X dominance coeff
	~Subpopulation(void);																			// destructor
	
	inline int DrawParentUsingFitness(void) const;													// draw an individual from the subpopulation based upon fitness
	inline int DrawParentEqualProbability(void) const;												// draw an individual from the subpopulation with equal probabilities
	inline int DrawFemaleParentUsingFitness(void) const;											// draw a female from the subpopulation based upon fitness; SEX ONLY
	inline int DrawFemaleParentEqualProbability(void) const;										// draw a female from the subpopulation  with equal probabilities; SEX ONLY
	inline int DrawMaleParentUsingFitness(void) const;												// draw a male from the subpopulation based upon fitness; SEX ONLY
	inline int DrawMaleParentEqualProbability(void) const;											// draw a male from the subpopulation  with equal probabilities; SEX ONLY
	
	void GenerateChildrenToFit(const bool p_parents_also);											// given the subpop size and sex ratio currently set for the child generation, make new genomes to fit
	inline IndividualSex SexOfIndividual(int p_individual_index);									// return the sex of the individual at the given index; uses child_generation_valid
	void UpdateFitness(std::vector<SLiMEidosBlock*> &p_fitness_callbacks);							// update the fitness lookup table based upon current mutations
	double FitnessOfParentWithGenomeIndices(int p_genome_index1, int p_genome_index2, std::vector<SLiMEidosBlock*> &p_fitness_callbacks);	// calculate the fitness of a given individual; the x dominance coeff is used only if the X is modeled
	double ApplyFitnessCallbacks(Mutation *p_mutation, int p_homozygous, double p_computed_fitness, std::vector<SLiMEidosBlock*> &p_fitness_callbacks, Genome *genome1, Genome *genome2);
	void SwapChildAndParentGenomes(void);															// switch to the next generation by swapping; the children become the parents
	
	//
	// Eidos support
	//
	void GenerateCachedSymbolTableEntry(void);
	inline EidosSymbolTableEntry *CachedSymbolTableEntry(void) { if (!self_symbol_) GenerateCachedSymbolTableEntry(); return self_symbol_; };
	
	virtual const EidosObjectClass *Class(void) const;
	virtual void Print(std::ostream &p_ostream) const;
	
	virtual EidosValue *GetProperty(EidosGlobalStringID p_property_id);
	virtual void SetProperty(EidosGlobalStringID p_property_id, EidosValue *p_value);
	virtual EidosValue *ExecuteInstanceMethod(EidosGlobalStringID p_method_id, EidosValue *const *const p_arguments, int p_argument_count, EidosInterpreter &p_interpreter);
};


inline __attribute__((always_inline)) int Subpopulation::DrawParentUsingFitness(void) const
{
#if DEBUG
	if (sex_enabled_)
		EIDOS_TERMINATION << "Subpopulation::DrawParentUsingFitness() called on a population for which sex is enabled" << eidos_terminate();
#endif
	
	return static_cast<int>(gsl_ran_discrete(gEidos_rng, lookup_parent_));
}

inline __attribute__((always_inline)) int Subpopulation::DrawParentEqualProbability(void) const
{
#if DEBUG
	if (sex_enabled_)
		EIDOS_TERMINATION << "Subpopulation::DrawParentEqualProbability() called on a population for which sex is enabled" << eidos_terminate();
#endif
	
	return static_cast<int>(gsl_rng_uniform_int(gEidos_rng, parent_subpop_size_));
}

// SEX ONLY
inline __attribute__((always_inline)) int Subpopulation::DrawFemaleParentUsingFitness(void) const
{
#if DEBUG
	if (!sex_enabled_)
		EIDOS_TERMINATION << "Subpopulation::DrawFemaleParentUsingFitness() called on a population for which sex is not enabled" << eidos_terminate();
#endif
	
	return static_cast<int>(gsl_ran_discrete(gEidos_rng, lookup_female_parent_));
}

// SEX ONLY
inline __attribute__((always_inline)) int Subpopulation::DrawFemaleParentEqualProbability(void) const
{
#if DEBUG
	if (!sex_enabled_)
		EIDOS_TERMINATION << "Subpopulation::DrawFemaleParentEqualProbability() called on a population for which sex is not enabled" << eidos_terminate();
#endif
	
	return static_cast<int>(gsl_rng_uniform_int(gEidos_rng, parent_first_male_index_));
}

// SEX ONLY
inline __attribute__((always_inline)) int Subpopulation::DrawMaleParentUsingFitness(void) const
{
#if DEBUG
	if (!sex_enabled_)
		EIDOS_TERMINATION << "Subpopulation::DrawMaleParentUsingFitness() called on a population for which sex is not enabled" << eidos_terminate();
#endif
	
	return static_cast<int>(gsl_ran_discrete(gEidos_rng, lookup_male_parent_)) + parent_first_male_index_;
}

// SEX ONLY
inline __attribute__((always_inline)) int Subpopulation::DrawMaleParentEqualProbability(void) const
{
#if DEBUG
	if (!sex_enabled_)
		EIDOS_TERMINATION << "Subpopulation::DrawMaleParentEqualProbability() called on a population for which sex is not enabled" << eidos_terminate();
#endif
	
	return static_cast<int>(gsl_rng_uniform_int(gEidos_rng, parent_subpop_size_ - parent_first_male_index_) + parent_first_male_index_);
}

inline IndividualSex Subpopulation::SexOfIndividual(int p_individual_index)
{
	if (!sex_enabled_)
	{
		return IndividualSex::kHermaphrodite;
	}
	else if (child_generation_valid)
	{
		if (p_individual_index < child_first_male_index_)
			return IndividualSex::kFemale;
		else
			return IndividualSex::kMale;
	}
	else
	{
		if (p_individual_index < parent_first_male_index_)
			return IndividualSex::kFemale;
		else
			return IndividualSex::kMale;
	}
}


#endif /* defined(__SLiM__subpopulation__) */




































































