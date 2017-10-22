def powtest(a,b):
	return hex(pow(a,b,0x5aeb35b))

pub=0x5aeb35bb230c8
rec=0x4722af9
priv=0x325ae71
res=0x5040b30

print powtest(priv,rec)
print powtest(rec,priv)
