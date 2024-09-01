using LoginWebAPI.DTOs.Register;
using Microsoft.AspNetCore.Mvc;

namespace LoginWebAPI.Controllers
{
    [Route("api/user")]
    public class UserController : Controller
    {

        private AccountContext _context;

        public UserController(AccountContext context)
        {
            _context = context;
        }

        [Route("register")]
        [HttpPost]
        public IActionResult Register([FromBody] ReqRegisterDto dto)
        {
            Account account = new Account() { UserName = dto.UserName, Password = dto.Password };
            _context.Account.Add(account);
            _context.SaveChanges();

            return Ok();
        }
    }
}
