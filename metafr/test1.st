program dokimi
{
	declare i,j,k enddeclare
	
	function foo(in a, inout b)
	{
		declare x,y enddeclare
		x := a+b;
		y := foo(in a, copy b);
		
		if(x<y){
			return (x);
		}
		else{
			return (y);
		}
	}
	
	do{
		i :=i+1;
		j :=0; 
		while(j<100){
			k := k+i*j;
		}
	}while(i<10);
}