library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.STD_LOGIC_UNSIGNED.all;
use IEEE.NUMERIC_STD.ALL;

entity CT_MIN is
	port(clk: in std_logic;
	i: in integer;
	o: inout integer := 0;
	c: out STD_LOGIC
	);
end entity;				   

architecture comp of CT_MIN is 
signal aux: integer := -1;
begin 
	process(clk)
	begin  	
		if aux = -1 then
			aux <= i;
		end if;
		if (CLK'EVENT and CLK = '1') then
			if aux = 59 then
				aux <= 0;
				c <= '1';
			else
				aux <= aux + 1;
				c <= '0';
			end if;
		end if;
		o <= aux;
	end process; 
end architecture;	

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.STD_LOGIC_UNSIGNED.all;
use IEEE.NUMERIC_STD.ALL;

entity MS_CT_MIN is
end entity;

architecture ARG_MS_CT_MIN of MS_CT_MIN is
component CT_MIN is
	port(clk: in std_logic;
	i: in integer;
	o: inout integer := 0;
	c: out STD_LOGIC);
end component;
signal clk,c: std_logic;
signal i: integer := 24;
signal o: integer;
begin
	UST: CT_MIN port map(clk => clk, i => i, o => o, c => c);
	STIM: process
	begin 
		CLK <= '0';
		wait for 50ns;
		CLK <= '1';
		wait for 50ns;
	end process;
end architecture;