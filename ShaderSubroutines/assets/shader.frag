#version 420

out vec4 oColor;

subroutine vec4 mySubroutine(vec4 p);

subroutine(mySubroutine)
vec4 myFunc1(vec4 p)
{
  return p * vec4(1.0, 0.0, 0.0, 1.0);
}

subroutine(mySubroutine)
vec4 myFunc2(vec4 p)
{
  return p * vec4(0.0, 1.0, 0.0, 1.0);
}

subroutine uniform mySubroutine uFuncPtr;

void main()
{
  oColor = uFuncPtr(vec4(1.0));
}
