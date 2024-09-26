~
	FIZZBUZZ 
	- noClueBruh
	- these are comments btw
~

IO::print('[ INPUT ] N = ');
int num = Sys::toInt(IO::readLine());
IO::println('---------');

int i = 1;
while i <= num do 
	function int isPerfectFive() 
		return i % 5 == 0;
	end
	
	int perfectFive = i % 5 == 0;
	
	if i % 3 == 0 do 
		if perfectFive do 
			IO::println('FizzBuzz');
		else  
			IO::println('Fizz');
		end 
	elseif perfectFive do 
		IO::println('Buzz');
	else 
		IO::println(string i);
	end
	
	i = i + 1;
end 