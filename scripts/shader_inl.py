import sys, os

fb = os.path.basename(sys.argv[1]);
fp = os.path.splitext(fb)

with open(sys.argv[2],'w') as f:
  f.write("namespace shaders::" + fp[0] + "::" + fp[1][1:] + " {\n")
  f.write("namespace {\n")
  f.write("const uint8_t spv[] = {\n")
  f.write("#include \"" + fb + ".spv.inl\"\n")
  f.write("};\n")
  f.write("const char glsl0[] = \n")
  f.write("#include \"" + fb + ".glsl.inl\"\n")
  f.write(";\n")
  f.write("const char *glsl[] = { glsl0 };\n")
  f.write("}\n")
  f.write("}\n")
