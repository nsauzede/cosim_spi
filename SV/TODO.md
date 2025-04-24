6. Interface Example
systemverilog
Copier
Modifier
interface simple_bus(input logic clk);
  logic [7:0] data;
  logic valid;
endinterface
7. Enum & Struct
systemverilog
Copier
Modifier
typedef enum logic [1:0] {IDLE, READ, WRITE, DONE} state_t;
typedef struct packed {
  logic [7:0] addr;
  logic [7:0] data;
} packet_t;
8. Assertions
systemverilog
Copier
Modifier
module check(input logic clk, valid);
  always @(posedge clk)
    assert (valid == 1) else $fatal("Valid is low");
endmodule
9. Parameters and Generate
systemverilog
Copier
Modifier
module gen_example #(parameter N = 4) (output logic [N-1:0] out);
  genvar i;
  generate
    for (i = 0; i < N; i++) begin
      assign out[i] = i;
    end
  endgenerate
endmodule
10. Classes and OOP
systemverilog
Copier
Modifier
class Packet;
  rand bit [7:0] data;
  function void display();
    $display("Data = %0d", data);
  endfunction
endclass

module tb_class;
  Packet p = new;
  initial begin
    p.randomize();
    p.display();
  end
endmodule
