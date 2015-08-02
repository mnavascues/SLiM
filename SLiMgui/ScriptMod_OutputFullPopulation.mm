//
//  ScriptMod_OutputFullPopulation.m
//  SLiM
//
//  Created by Ben Haller on 3/21/15.
//  Copyright (c) 2015 Philipp Messer.  All rights reserved.
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


#import "ScriptMod_OutputFullPopulation.h"

#include <string>
#include <map>
#include <vector>


@implementation ScriptMod_OutputFullPopulation

- (NSString *)sheetTitle
{
	return @"Output Full Population";
}

- (NSString *)scriptSectionName
{
	return @"#OUTPUT";
}

- (NSString *)sortingGrepPattern
{
	return [ScriptMod scientificIntSortingGrepPattern];
}

- (void)configSheetLoaded
{
	// set initial control values
	[generationTextField setStringValue:[NSString stringWithFormat:@"%d", controller->sim->generation_]];
	[filenameTextField setStringValue:@""];
	
	[super configSheetLoaded];
}

- (IBAction)validateControls:(id)sender
{
	// Determine whether we have valid inputs in all of our fields
	validInput = YES;
	
	BOOL generationValid = [ScriptMod validIntWithScientificNotationValueInTextField:generationTextField withMin:1 max:SLIM_MAX_GENERATION];
	validInput = validInput && generationValid;
	[generationTextField setBackgroundColor:[ScriptMod backgroundColorForValidationState:generationValid]];
	
	BOOL filenameValid = ([[filenameTextField stringValue] length] == 0) || [ScriptMod validFilenameInTextField:filenameTextField];
	validInput = validInput && filenameValid;
	[filenameTextField setBackgroundColor:[ScriptMod backgroundColorForValidationState:filenameValid]];
	
	// determine whether we will need to recycle to simulation to make the change take effect
	needsRecycle = ((int)[generationTextField doubleValue] < controller->sim->generation_);		// handle scientific notation
	
	// now we call super, and it uses validInput and needsRecycle to fix up the UI for us
	[super validateControls:sender];
}

- (NSString *)scriptLineWithExecute:(BOOL)executeNow
{
	NSString *targetGeneration = [generationTextField stringValue];
	int targetGenerationInt = (int)[targetGeneration doubleValue];
	NSString *filename = [filenameTextField stringValue];
	
	if (executeNow)
	{
		if (needsRecycle)
		{
			// queue a -recycle: operation to happen after we're done modifying the script
			[controller performSelector:@selector(recycle:) withObject:nil afterDelay:0.0];
		}
		else
		{
			// insert the event into the simulation's event map
			NSString *param1 = filename;
			std::vector<std::string> event_parameters;
			
			if ([filename length])
				event_parameters.push_back(std::string([param1 UTF8String]));
			
			Event *new_event_ptr = new Event('A', event_parameters);
			
			controller->sim->events_.insert(std::pair<const int,Event*>(targetGenerationInt, new_event_ptr));
		}
	}
	
	if ([filename length])
		return [NSString stringWithFormat:@"%@ A %@", targetGeneration, filename];
	else
		return [NSString stringWithFormat:@"%@ A", targetGeneration];
}

@end















































































