# Intrinsics

## printf

Integer:
```
%1 = load i32* %bar
%2 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @.str, i32 0, i32 0), i32 %1)
```

Float:
```
%1 = load float* %bar
%2 = fpext float %1 to double
%3 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @.str, i32 0, i32 0), double %2)
```
