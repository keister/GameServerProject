using Microsoft.EntityFrameworkCore;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace LoginWebAPI
{
    public class AccountContext : DbContext
    {


        public AccountContext(DbContextOptions<AccountContext> options) : base(options)
        {
        }
        public DbSet<Account> Account { get; set; }
    }


    [Table(name: "account")]
     public class Account
    {
        [Key]
        
        [Column(name: "id")]
        public UInt64 Id {get; set; }

        [Column(name: "username")]
        [Required(ErrorMessage = "사용자 아이디를 입력하세요.")]
        public string UserName {get; set; }

        
        [Column(name: "password")]
        [Required(ErrorMessage = "사용자 비밀번호를 입력하세요.")]
        public string Password {get; set; }

    }
}
