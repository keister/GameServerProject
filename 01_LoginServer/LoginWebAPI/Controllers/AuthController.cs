using System.IdentityModel.Tokens.Jwt;
using LoginWebAPI.DTOs.Auth;
using LoginWebAPI.Utils;
using Microsoft.AspNetCore.Mvc;



namespace LoginWebAPI.Controllers
{

    [Route("api/auth")]
    [ApiController]
    public class AuthController : ControllerBase
    {


        private AccountContext _context;
        private JwtUtils _jwtUtils;

        public AuthController(AccountContext context, JwtUtils jwtUtils)
        {
            _context = context;
            _jwtUtils = jwtUtils;
        }

        [Route("verify-token")]
        [HttpPost]
        public IActionResult VerifyToken([FromBody] VerifyTokenReqDto dto)
        {
            var result = _jwtUtils.ValidateToken(dto.Token);

            if (result == null)
            {
                return Unauthorized(new { error = "Id not found" });
            }

            return Ok(result);
        }
         
        [Route("login")]
        [HttpPost]
        public IActionResult Login([FromBody] LoginReqDto dto)
        {
            var acc = _context.Account.FirstOrDefault(a => a.UserName == dto.UserName);


            if (acc == null)
            { 
                return Unauthorized(new { error = "Id not found" });
            }

            

            if (acc.Password != dto.Password)
            {
                return Unauthorized(new { error = "Invalid password." });
            }

            string token = _jwtUtils.GenerateToken(acc);

            LoginResDto resDto = new() { Id = acc.Id, Token = token };

            return Ok(resDto);
        }
    }
}
