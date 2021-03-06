/*******************************************************************************
 *
 * (c) Copyright IBM Corp. 1991, 2015
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

#include "GlobalCollector.hpp"
#include "GlobalCollectorDelegate.hpp"

void
MM_GlobalCollector::internalPostCollect(MM_EnvironmentBase* env, MM_MemorySubSpace* subSpace)
{
	_delegate.postCollect(env, subSpace);
}

bool
MM_GlobalCollector::isTimeForGlobalGCKickoff()
{
	return _delegate.isTimeForGlobalGCKickoff();
}

/**
 * Abort any currently active garbage collection activity.
 */
void
MM_GlobalCollector::abortCollection(MM_EnvironmentBase* env, CollectionAbortReason reason)
{
}

