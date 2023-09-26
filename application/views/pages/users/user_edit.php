<form action="" method="post">
    <div class="container mt-5">
        <div class="row">
            <div class="form-group col-lg-6">
                <label for="num_mat">Num Matricule:</label>
                <input type="text" class="form-control" id="num_mat">
            </div>
            <div class="form-grou col-lg-6">
                <label for="nom">Nom:</label>
                <input type="text" class="form-control" id="nom">
            </div>
        </div>
        
        <div class="form-group">
            <label for="">Profile</label>
            <select class="form-control" id="code_profile">
                <option value="profil1">profile 1</option>
                <option value="profil2">profile 2</option>
                <option value="profil3">profile 3</option>
            </select>
        </div>
        <div class="container">
            <div class="text-center">
                <button type="reset" class="btn btn-danger">Reinitialiser</button>
                <button type="submit" class="btn btn-success">Ajouter</button>
            </div>
        </div>
    </div>
</form>