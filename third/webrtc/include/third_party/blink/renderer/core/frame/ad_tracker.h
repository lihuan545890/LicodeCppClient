// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_AD_TRACKER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_AD_TRACKER_H_

#include "base/feature_list.h"
#include "base/macros.h"
#include "third_party/blink/renderer/core/probe/async_task_id.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/loader/fetch/fetch_initiator_info.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/wtf/hash_map.h"
#include "third_party/blink/renderer/platform/wtf/hash_set.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"
#include "v8/include/v8.h"

namespace blink {

class Document;
class ExecutionContext;
class LocalFrame;
class ResourceRequest;
enum class ResourceType : uint8_t;

namespace probe {
class CallFunction;
class ExecuteScript;
}  // namespace probe

namespace features {
CORE_EXPORT extern const base::Feature kAsyncStackAdTagging;
CORE_EXPORT extern const base::Feature kTopOfStackAdTagging;
}

// Tracker for tagging resources as ads based on the call stack scripts.
// The tracker is maintained per local root.
class CORE_EXPORT AdTracker : public GarbageCollected<AdTracker> {
 public:
  enum class StackType { kBottomOnly, kBottomAndTop };
  // Finds an AdTracker for a given ExecutionContext.
  static AdTracker* FromExecutionContext(ExecutionContext*);

  static bool IsAdScriptExecutingInDocument(
      Document* document,
      StackType stack_type = StackType::kBottomAndTop);

  // Instrumenting methods.
  // Called when a script module or script gets executed from native code.
  void Will(const probe::ExecuteScript&);
  void Did(const probe::ExecuteScript&);

  // Called when a function gets called from native code.
  void Will(const probe::CallFunction&);
  void Did(const probe::CallFunction&);

  // Called when a subresource request is about to be sent or is redirected.
  // Returns true if any of the following are true:
  // - the resource is loaded in an ad iframe
  // - |known_ad| is true
  // - ad script is in the v8 stack and the resource was not requested by CSS.
  // Virtual for testing.
  virtual bool CalculateIfAdSubresource(
      ExecutionContext* execution_context,
      const ResourceRequest& request,
      ResourceType resource_type,
      const FetchInitiatorInfo& initiator_info,
      bool known_ad);

  // Called when an async task is created. Check at this point for ad script on
  // the stack and annotate the task if so.
  void DidCreateAsyncTask(probe::AsyncTaskId* task);

  // Called when an async task is eventually run.
  void DidStartAsyncTask(probe::AsyncTaskId* task);

  // Called when the task has finished running.
  void DidFinishAsyncTask(probe::AsyncTaskId* task);

  // Returns true if any script in the pseudo call stack has previously been
  // identified as an ad resource, if the current ExecutionContext is a known ad
  // execution context, or if the script at the top of isolate's
  // stack is ad script. Whether to look at just the bottom of the
  // stack or the top and bottom is indicated by |stack_type|. kBottomAndTop is
  // generally best as it catches more ads, but if you're calling very
  // frequently then consider just the bottom of the stack for performance sake.
  bool IsAdScriptInStack(StackType stack_type);

  virtual void Trace(Visitor*) const;

  void Shutdown();
  explicit AdTracker(LocalFrame*);
  virtual ~AdTracker();

 protected:
  // Protected for testing.
  virtual String ScriptAtTopOfStack();
  virtual ExecutionContext* GetCurrentExecutionContext();

 private:
  friend class FrameFetchContextSubresourceFilterTest;
  friend class AdTrackerSimTest;
  friend class AdTrackerTest;

  // |script_name| will be empty in the case of a dynamically added script with
  // no src attribute set. |script_id| won't be set for module scripts in an
  // errored state or for non-source text modules.
  void WillExecuteScript(ExecutionContext*,
                         const String& script_name,
                         int script_id);
  void DidExecuteScript();
  bool IsKnownAdScript(ExecutionContext* execution_context, const String& url);
  void AppendToKnownAdScripts(ExecutionContext&, const String& url);

  Member<LocalFrame> local_root_;

  // Each time v8 is started to run a script or function, this records if it was
  // an ad script. Each time the script or function finishes, it pops the stack.
  Vector<bool> stack_frame_is_ad_;

  uint32_t num_ads_in_stack_ = 0;

  // The set of ad scripts detected outside of ad-frame contexts. Scripts with
  // no name (i.e. URL) use a String created by GenerateFakeUrlFromScriptId().
  HeapHashMap<WeakMember<ExecutionContext>, HashSet<String>> known_ad_scripts_;

  // The number of ad-related async tasks currently running in the stack.
  uint32_t running_ad_async_tasks_ = 0;

  // True if the AdTracker looks not only at the current V8 stack for ad script
  // but also at the previous asynchronous stacks that caused this current
  // callstack to run (e.g., registered callbacks).
  const bool async_stack_enabled_;

  // True if the TopOfStack experiment is running, which forces the AdTracker to
  // ignore the bottom of stack frames when looking for ad script.
  const bool top_of_stack_only_;

  DISALLOW_COPY_AND_ASSIGN(AdTracker);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_AD_TRACKER_H_
