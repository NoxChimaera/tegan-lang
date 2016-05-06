echo "BUILD:"
dist/hello example/main.t dist/out.3ac
echo "RUN:"
lli dist/out.3ac
