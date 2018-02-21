#ifndef __CPSC489_TESTS_H
#define __CPSC489_TESTS_H

BOOL BeginTest(int cmd);
void EndTest(void);
void RenderTest(real32 dt);
BOOL IsTestActive(void);
int GetActiveTest(void);

#endif
