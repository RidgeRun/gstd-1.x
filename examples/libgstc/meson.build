# Compile examples
app_examples = [
  'dynamic_property_change',
  'gapless_playback',
  'mp4_recording',
  'simple_pipeline',
]

foreach app : app_examples
  executable(app, '@0@.c'.format(app),
    include_directories: [configinc,lib_gstc_inc_dir],
    dependencies : [gst_client_deps,lib_gstc_dep],
    install: false)
endforeach
