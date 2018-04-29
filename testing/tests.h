#ifndef __CS489_TESTS_H
#define __CS489_TESTS_H

BOOL BeginTest(int cmd);
void EndTest(void);
void UpdateTest(real32 dt);
void RenderTest(void);
BOOL IsTestActive(void);
int GetActiveTest(void);

#endif
