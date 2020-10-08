/*
 * Copyright (C) 2012 Google Inc.  All rights reserved.
 * Copyright (C) 2013 Apple Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_ENCRYPTEDMEDIA_MEDIA_KEY_MESSAGE_EVENT_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_ENCRYPTEDMEDIA_MEDIA_KEY_MESSAGE_EVENT_H_

#include "third_party/blink/renderer/bindings/modules/v8/v8_media_key_message_event_init.h"
#include "third_party/blink/renderer/modules/event_modules.h"

namespace blink {

class DOMArrayBuffer;

class MediaKeyMessageEvent final : public Event {
  DEFINE_WRAPPERTYPEINFO();

 public:
  MediaKeyMessageEvent();
  MediaKeyMessageEvent(const AtomicString& type,
                       const MediaKeyMessageEventInit* initializer);
  ~MediaKeyMessageEvent() override;

  static MediaKeyMessageEvent* Create() {
    return MakeGarbageCollected<MediaKeyMessageEvent>();
  }

  static MediaKeyMessageEvent* Create(
      const AtomicString& type,
      const MediaKeyMessageEventInit* initializer) {
    return MakeGarbageCollected<MediaKeyMessageEvent>(type, initializer);
  }

  const AtomicString& InterfaceName() const override;

  String messageType() const { return message_type_; }
  DOMArrayBuffer* message() const { return message_.Get(); }

  void Trace(Visitor*) const override;

 private:
  String message_type_;
  Member<DOMArrayBuffer> message_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_ENCRYPTEDMEDIA_MEDIA_KEY_MESSAGE_EVENT_H_
