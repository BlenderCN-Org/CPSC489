#include "../stdafx.h"
#include "../stdres.h"
#include "../errors.h"
#include "../gfx.h"
#include "tests.h"

#include "sk_axes.h"

typedef BOOL (*InitFunc)(void);
typedef void (*FreeFunc)(void);

static int active_test = -1;

BOOL BeginTest(int cmd)
{
 return TRUE;
}

void EndTest(void)
{
}

BOOL IsTestActive(void)
{
 return (active_test == -1 ? FALSE : TRUE);
}

int GetActiveTest(void)
{
 return active_test;
}