// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SHELL_GPU_GPU_SURFACE_GL_SKIA_H_
#define SHELL_GPU_GPU_SURFACE_GL_SKIA_H_

#include <functional>
#include <memory>

#include "flutter/common/graphics/gl_context_switch.h"
#include "flutter/flow/embedded_views.h"
#include "flutter/flow/surface.h"
#include "flutter/fml/macros.h"
#include "flutter/fml/memory/weak_ptr.h"
#include "flutter/shell/gpu/gpu_surface_gl_delegate.h"
#include "third_party/skia/include/gpu/GrDirectContext.h"

namespace flutter {

class GPUSurfaceGLSkia : public Surface {
 public:
  static sk_sp<GrDirectContext> MakeGLContext(GPUSurfaceGLDelegate* delegate);

  GPUSurfaceGLSkia(GPUSurfaceGLDelegate* delegate, bool render_to_surface);

  // Creates a new GL surface reusing an existing GrDirectContext.
  GPUSurfaceGLSkia(sk_sp<GrDirectContext> gr_context,
                   GPUSurfaceGLDelegate* delegate,
                   bool render_to_surface);

  // |Surface|
  ~GPUSurfaceGLSkia() override;

  // |Surface|
  bool IsValid() override;

  // |Surface|
  std::unique_ptr<SurfaceFrame> AcquireFrame(const SkISize& size) override;

  // |Surface|
  SkMatrix GetRootTransformation() const override;

  // |Surface|
  GrDirectContext* GetContext() override;

  // |Surface|
  std::unique_ptr<GLContextResult> MakeRenderContextCurrent() override;

  // |Surface|
  bool ClearRenderContext() override;

  // |Surface|
  bool AllowsDrawingWhenGpuDisabled() const override;

 private:
  bool CreateOrUpdateSurfaces(const SkISize& size);

  sk_sp<SkSurface> AcquireRenderSurface(
      const SkISize& untransformed_size,
      const SkMatrix& root_surface_transformation);

  bool PresentSurface(const SurfaceFrame& frame, SkCanvas* canvas);

  GPUSurfaceGLDelegate* delegate_;
  sk_sp<GrDirectContext> context_;
  sk_sp<SkSurface> onscreen_surface_;
  /// FBO backing the current `onscreen_surface_`.
  uint32_t fbo_id_ = 0;
  // Private variable used to keep track of the current FBO's existing damage.
  SkIRect existing_damage_ = SkIRect::MakeEmpty();
  bool context_owner_ = false;
  // TODO(38466): Refactor GPU surface APIs take into account the fact that an
  // external view embedder may want to render to the root surface. This is a
  // hack to make avoid allocating resources for the root surface when an
  // external view embedder is present.
  const bool render_to_surface_ = true;
  bool valid_ = false;
  // Partial repaint is on by default.
  bool supports_partial_repaint_ = true;

  // WeakPtrFactory must be the last member.
  fml::TaskRunnerAffineWeakPtrFactory<GPUSurfaceGLSkia> weak_factory_;
  FML_DISALLOW_COPY_AND_ASSIGN(GPUSurfaceGLSkia);
};

}  // namespace flutter

#endif  // SHELL_GPU_GPU_SURFACE_GL_SKIA_H_
